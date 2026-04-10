#include <charconv>
#include <chrono>
#include <iostream>
#include <lsp/connection.h>
#include <lsp/io/socket.h>
#include <lsp/io/standardio.h>
#include <lsp/messagehandler.h>
#include <lsp/types.h>
#include <lsp/messages.h>
#include <string_view>
#include <thread>

#include "AlberichDistro/Interpreter.h"

// Basiert auf dem Standard Language Server Beispiel aus dem LSP Framework

namespace{

/*
 * Helpers to print the message method and payload
 */

template<typename MessageType>
void printMessageMethod()
{
	const auto type = lsp::message::IsNotification<MessageType> ? "notification" : "request";
	std::cerr << "Server received " << type << " '" << MessageType::Method << '\'' << std::endl;
}

template<typename MessageType>
void printMessagePayload(const typename MessageType::Params& params)
{
	const auto json = lsp::toJson(typename MessageType::Params(params));
	std::cerr << "payload: " << lsp::json::stringify(json, true) << std::endl;
}

template<typename MessageType>
void printMessage(const typename MessageType::Params& params)
{
	printMessageMethod<MessageType>();
	printMessagePayload<MessageType>(params);
}

template<typename MessageType>
void printMessage()
{
	printMessageMethod<MessageType>();
}

/*
 * Callback registration
 */

thread_local bool g_running = false;

std::string uriToPath(const std::string_view& uri)
{
	std::string path = std::string(lsp::Uri::parse(uri).path());

	#ifdef _WIN32
		path = (path.size() >= 3 && path[0] == '/' && path[2] == ':') ? path.substr(1) : path;
	#endif

	return path;
}

void registerCallbacks(lsp::MessageHandler& messageHandler, lsp::Connection& connection){

	auto state = std::make_shared<LspState>();

	state->keywords     = { "if","xIf","rIf","nIf","else","requires","assert","fetch","script","backend","decl","for","while","return","break","continue","static","struct", "ref", "this" };
    state->typeKeywords = { "void", "bool", "int", "double", "args" };

	messageHandler.add<lsp::requests::Initialize>(
		[](lsp::requests::Initialize::Params&& params)
		{
			printMessage<lsp::requests::Initialize>(params);

			/*
			 * Respond with an InitializeResult containing some basic server info and capabilities
			 */

			return lsp::requests::Initialize::Result{
				.capabilities = {
					.positionEncoding = lsp::PositionEncodingKind::UTF16,
					.textDocumentSync = lsp::TextDocumentSyncOptions{
						.openClose = true,
						.change    = lsp::TextDocumentSyncKind::Full,
						.save      = true
					},
					.completionProvider = lsp::CompletionOptions{
						.triggerCharacters = lsp::Array<lsp::String>{ 
							"+", "-", "*", "/", "=", "<", ">", "!", "&", "|", 
							"^", "%", "~", ".", ":", "?", "@", "#", "\\"
						}
					},
					// .semanticTokensProvider = lsp::SemanticTokensOptions{
                    //     .legend = lsp::SemanticTokensLegend{
                    //         // Reihenfolge = Index! keyword=0, type=1, variable=2, function=3
                    //         .tokenTypes   = { "keyword", "type", "variable", "function" },
                    //         .tokenModifiers = {}
                    //     },
                    //     .full = true
                    // },
					.hoverProvider = true,
				},
				.serverInfo = lsp::InitializeResultServerInfo{
					.name    = "Language Server",
					.version = "1.0.0"
				},
			};
		}
	)

	.add<lsp::requests::TextDocument_Completion>(
		[state](lsp::requests::TextDocument_Completion::Params&& params)
		{
			return std::async(std::launch::deferred,
				[state, params = std::move(params)]()
				{	
					const std::string path = uriToPath(params.textDocument.uri.toString());
					
					const LSPData data = getLSPData(path);
					state->applyLSPData(data);

					// Contains Case Insensitive -> Groß-/Kleinschreibung wird nicht beachtet
					auto containsCI = [](const std::string& haystack, const std::string& needle) {
						
						if(needle.empty()) return true;
						auto it = std::search(
							haystack.begin(), haystack.end(),
							needle.begin(),   needle.end(),
							[](char a, char b) { return std::tolower(a) == std::tolower(b); }
						);
						return it != haystack.end();
					};

					const auto& text = state->documents[params.textDocument.uri.toString()];
					const auto  line = params.position.line;
					const auto  col  = params.position.character;
					
					// Zeile finden
					size_t lineStart = 0;
					for(uint32_t i = 0; i < line; i++){

						lineStart = text.find('\n', lineStart);
						if(lineStart == std::string::npos) break;
						lineStart++;
					}

					auto isWordChar = [](char c) {
						return !std::isspace(c) && c != '(' && c != ')' && c != '{' && c != '}' && c != '[' && c != ']' && c != ',' && c != ';';
					};

					size_t wordStart = lineStart + col;
					while(wordStart > 0 && isWordChar(text[wordStart-1]))
						wordStart--;

					const auto currentWord = text.substr(wordStart, lineStart + col - wordStart);
					std::cerr << "Completing: " << currentWord << std::endl;

					std::vector<lsp::CompletionItem> items;

					for(const auto& kw : state->keywords)
						if(containsCI(kw, currentWord))
							items.push_back({ .label = kw, .kind = lsp::CompletionItemKind::Keyword, .detail = "keyword", .sortText = "1_" });

					for(const auto& t : state->typeKeywords)
						if(containsCI(t, currentWord))	
							items.push_back({ .label = t, .kind = lsp::CompletionItemKind::Class, .detail = "type", .sortText = "2_" });

					for(const auto& [key, detail] : data.constKeywords)
						if(containsCI(key.first, currentWord))	
							items.push_back({ .label = key.first, .kind = lsp::CompletionItemKind::Constant, .detail = detail.first, .sortText = "3_" });
				
					for(const auto& [key, details] : data.functions)
						if(containsCI(key.first, currentWord))
							items.push_back({ .label = key.first, .kind = lsp::CompletionItemKind::Function, .detail = "function " + details.first,
								.sortText = "5_", .insertText = key.first + "($1)", .insertTextFormat = lsp::InsertTextFormat::Snippet });

					for(const auto& [key, details] : data.variables)
						if(containsCI(key.first, currentWord))
							items.push_back({ .label = key.first, .kind = lsp::CompletionItemKind::Variable, .detail = details.first, .sortText = "6_" });
					
					// Die meisten Editoren sind nicht darauf ausgelegt Operatoren zu autovervollständigen
					// Da Baldur aber mit einer umfangreichen, unkonventionellen, dynamisch anpassbaren Operatorenauswahl arbeitet ist das hier nötigt
					// Um die korrekte Behandlung zu gewährleisten muss die Autovervollständigung beser vorsortiert werden und das bestehende Wort bei der
					// Autovervollständigung ersetzt
					// ansonsten wird der Autovervollständigungstext des Operators einfach hinten angehängt 
					for(const auto& ops : data.operators) {

						if(ops.contains(currentWord)) {

							// Item
							lsp::CompletionItem item{
								.label   = ops,
								.kind    = lsp::CompletionItemKind::Operator,
								.detail  = "operator",
								.sortText = "4_"
							};

							// Bereich von wordStart bis Cursor Position
							lsp::Range range{ .start = lsp::Position{
												.line = static_cast<int32_t>(line),
												.character = static_cast<int32_t>(wordStart - lineStart) },
											  .end = lsp::Position{
												.line = static_cast<int32_t>(line),
												.character = static_cast<int32_t>(col) }};

							// Da die Autovervollständigung erst wieder bereinigt funktioniert wenn hinter das zu vervollständigende
							// Wort durch ein Leerzeichen vom Operatore getrennt ist kann hier auch direkt automatisch ein Leerzeichen
							// hinter den Operator geschrieben werden über '.newText = ops + " "'
							lsp::TextEdit textEdit{ .range   = range, .newText = ops };

							item.textEdit = std::move(textEdit);
							items.push_back(std::move(item));
						}
					}

					return lsp::requests::TextDocument_Completion::Result(std::move(items));
				}
			);
		}
	)

	.add<lsp::notifications::TextDocument_DidChange>(
		[state](lsp::notifications::TextDocument_DidChange::Params&& params)
		{
			const auto& change = params.contentChanges[0];

			std::visit([&](const auto& c){
				state->documents[params.textDocument.uri.toString()] = c.text;
			}, change);
		}
	)

	.add<lsp::notifications::TextDocument_DidOpen>(
		[state](lsp::notifications::TextDocument_DidOpen::Params&& params)
		{
			state->documents[params.textDocument.uri.toString()] = params.textDocument.text;
		}
	)

	.add<lsp::requests::TextDocument_Hover>(
		[state](lsp::requests::TextDocument_Hover::Params&& params)
		{
			printMessage<lsp::requests::TextDocument_Hover>(params);

			/*
			 * Handle the request asynchronously.
			 * It is executed in a worker thread by the message handler.
			 * This means a deferred future can be used and it is not necessary to spawn extra threads.
			 */
			return std::async(std::launch::deferred,
				[state, params = std::move(params)](){

					const std::string path = uriToPath(params.textDocument.uri.toString());
					
					const LSPData data = getLSPData(path);
					state->applyLSPData(data);

					// Text aus Cache holen statt Datei öffnen
					const auto& text = state->documents[params.textDocument.uri.toString()];

					const auto line = params.position.line;
					const auto col  = params.position.character;

					// Zeile finden
					size_t lineStart = 0;
					for(uint32_t i = 0; i < line; i++){
						lineStart = text.find('\n', lineStart);
						if(lineStart == std::string::npos)
							return lsp::requests::TextDocument_Hover::Result{};
						lineStart++;
					}

					// Wortgrenzen
					size_t wordStart = lineStart + col;
					size_t wordEnd   = lineStart + col;

					auto isWordChar = [](char c) {
						return !std::isspace(c) && c != '(' && c != ')' && c != '{' && c != '}' && c != '[' && c != ']' && c != ',' && c != ';';
					};

					while(wordStart > 0 && isWordChar(text[wordStart-1]))
						wordStart--;

					while(wordEnd < text.size() && isWordChar(text[wordEnd]))
						wordEnd++;

					const auto word = text.substr(wordStart, wordEnd - wordStart);
					std::cerr << "Hover " << word << "\n";

					std::string hoverContent;

					for(const auto& kw : state->keywords)
						if(kw == word)
							hoverContent += "- 🔑 **keyword** `" + word + "`\n";

					for(const auto& t : state->typeKeywords)
						if(t == word)
							hoverContent += "- 🏷️ **type** `" + word + "`\n";

					for(const auto& [key, detail] : data.constKeywords)
						if(key.first == word)	
							hoverContent += "- 💈 **const** `" + detail.second + "`\n";

					for(const auto& [key, details] : data.functions)
						if(key.first == word)
							hoverContent += "- ⚙️ **function** `" + details.second + "`\n";

					for(const auto& [key, details] : data.variables)
						if(key.first == word)
							hoverContent += "- 📦 **variable** `" + details.second + "`\n";

					for(const auto& ops : data.operators)
						if(ops == word)
							hoverContent += "- 🔧 **operator** `" + ops + "`\n";

					lsp::MarkupContent mc{
						.kind = lsp::MarkupKind::Markdown,
						.value = hoverContent
					};

					auto hover = lsp::Hover{
						.contents = mc
					};
					return lsp::requests::TextDocument_Hover::Result(std::move(hover));
				}
			);
		}
	)

	// .add<lsp::requests::TextDocument_Hover>(
	// 	[](lsp::requests::TextDocument_Hover::Params&& params)
	// 	{
	// 		printMessage<lsp::requests::TextDocument_Hover>(params);

	// 		/*
	// 		 * Handle the request asynchronously.
	// 		 * It is executed in a worker thread by the message handler.
	// 		 * This means a deferred future can be used and it is not necessary to spawn extra threads.
	// 		 */
	// 		return std::async(std::launch::deferred,
	// 			[params = std::move(params)]()
	// 			{
	// 				// simulate longer running task
	// 				// std::this_thread::sleep_for(std::chrono::seconds(2));

	// 				// return the result
	// 				// TextDocument_Hover::Result is NullOr<Hover>
	// 				auto hover = lsp::Hover{
	// 					.contents = "Hover result"
	// 				};
	// 				return lsp::requests::TextDocument_Hover::Result(std::move(hover));
	// 			}
	// 		);
	// 	}
	.add<lsp::requests::Shutdown>(
		[]()
		{
			printMessage<lsp::requests::Shutdown>();
			return lsp::requests::Shutdown::Result();
		}
	).add<lsp::notifications::Exit>(
		[]()
		{
			printMessage<lsp::notifications::Exit>();
			g_running = false;
		}
	);
}

/*
 * Message processing loop
 */

void runLanguageServer(lsp::io::Stream& io)
{
	try
	{
		auto connection     = lsp::Connection(io);
		auto messageHandler = lsp::MessageHandler(connection);
		registerCallbacks(messageHandler, connection);

		g_running = true;

		while(g_running)
			messageHandler.processIncomingMessages();
	}
	catch(const std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
}

/*
 * Socket server
 */

void runSocketServer(unsigned short port)
{
	std::cerr << "Waiting for incoming connections..." << std::endl;

	auto socketListener = lsp::io::SocketListener(port);

	while(socketListener.isReady())
	{
		auto socket = socketListener.listen();

		if(!socket.isOpen())
			break;

		std::cerr << "Accepted connection" << std::endl;

		auto thread = std::thread(
			[socket = std::move(socket)]() mutable
			{
				runLanguageServer(socket);
			}
		);
		thread.detach();
	}
}

/*
 * stdio server
 */

void runStdioServer()
{
	runLanguageServer(lsp::io::standardIO());
}

/*
 * Argument parsing
 */

std::optional<unsigned short> parsePortArg(int argc, char** argv)
{
	constexpr auto PortArg = std::string_view("--port=");

	for(int i = 1; i < argc; ++i)
	{
		const auto arg = std::string_view(argv[i]);

		if(arg.starts_with(PortArg))
		{
			unsigned short port;
			const auto portStr = arg.substr(PortArg.size());
			const auto [ptr, ec] = std::from_chars(portStr.data(), portStr.data() + portStr.size(), port);
			(void)ptr;

			if(ec == std::errc{})
				return port;
		}
		else
		{
			std::cerr << "Unknown argument: " << arg << std::endl;
		}
	}

	return std::nullopt;
}

} // namespace

int main(int argc, char** argv)
{
	try
	{
		const auto port = parsePortArg(argc, argv);

		if(!port.has_value())
		{
			std::cerr << "Starting stdio server - Launch with '--port=<portnum>' to run a socket server" << std::endl;
			runStdioServer();
		}
		else
		{
			std::cerr << "Starting socket server on port " << *port << std::endl;
			runSocketServer(*port);
		}

		std::cerr << "Exiting" << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}