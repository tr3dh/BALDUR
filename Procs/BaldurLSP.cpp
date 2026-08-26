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

#include "WinHandle/WinCmd.h"

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

LSPData provideLSPCache(const std::string& filePath){

	LSPData data = getLSPData(filePath);

	// Wenn valider LSPCache gefunden wurde
	if(!data.isEmpty()){ return data; }
		
	//
	std::cerr << "Kein gültiger LSPCache für " << filePath << " gefunden - Checke Default Cache..." << std::endl;

	//
	std::string defaultScriptPath = (fs::path(filePath).parent_path() / "__default.bld").string();
	data = getLSPData(defaultScriptPath);
	
	// default Cache valide
	if(!data.isEmpty()){ return data; }

	//
	std::cerr << "Kein gültiger default LSPCache (für " << defaultScriptPath << ") gefunden - Stelle Default Cache her..." << std::endl;

	// Pfad des Interpreters vorrausgesetzt er befindet sich im gleichen Verzeichnis wie der LS
	std::string interp = (fs::path(getExecutableDir()) / "Baldur").string();

	// Default Skript wird erzeugt
	std::ofstream(defaultScriptPath) << "slog(\"Default LSPCache erzeugt\")\n";

	// Interpreter führt erzeugtes Default Skript aus, dabei wird eine default LSP Cache erzeugt werden der bei fehlen eines Dateispezifischen
	// LSP Caches geladen werden kann und den Language Support gewährleisten
	std::string cmd = interp + " execute " + defaultScriptPath;
	streamWinCommand(cmd, [&](const char* callback){ std::cerr << callback; });

	// Default Skript wird wieder gelöscht
	std::error_code ec;
	fs::remove(defaultScriptPath, ec);
	if (ec) { std::cerr << "Default Skript konnte nicht gelöscht werden" << ec.message() << std::endl; }

	// LSPCache final aus erzeugtem Default Cache Laden
	data = getLSPData(defaultScriptPath);

	if(data.isEmpty()){ std::cerr << "Keine LSPCache gefunden" << std::endl; }

	return data;
}

/*
 * Callback registration
 */

thread_local bool g_running = false;

void registerCallbacks(lsp::MessageHandler& messageHandler, lsp::Connection& connection){

	auto state = std::make_shared<LspState>();

	state->keywords     = { "if","xIf","rIf","nIf","else","requires","assert","fetch","script","backend","decl","for","while","return","break","continue","static","struct", "ref", "this", "uniform", "defer" };
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
					.hoverProvider = true,
					.definitionProvider = true,
					.referencesProvider = true,
					.semanticTokensProvider = lsp::SemanticTokensOptions{
                        .legend = lsp::SemanticTokensLegend{
                            // Reihenfolge : keyword=0, type=1, variable=2, function=3
                            .tokenTypes   = { "keyword", "type", "enumMember", "function", "variable", "operator" },
                            .tokenModifiers = {}
                        },
                        .full = true
                    },
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
					
					const LSPData data = provideLSPCache(path);
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

					const auto& text = state->documents[path];
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
							items.push_back({ .label = key.first, .kind = lsp::CompletionItemKind::Variable, .detail = details.first, .sortText = "8_" });

					for(const auto& [key, details] : data.staticVariables)
						if(containsCI(key.first, currentWord))
							items.push_back({ .label = key.first, .kind = lsp::CompletionItemKind::Variable, .detail = details.first, .sortText = "6_" });

					for(const auto& [key, details] : data.memberVariables)
						if(containsCI(key.first, currentWord))
							items.push_back({ .label = key.first, .kind = lsp::CompletionItemKind::Variable, .detail = details.first, .sortText = "7_" });
					
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
				const std::string path = uriToPath(params.textDocument.uri.toString());
				state->documents[path] = c.text;
			}, change);
		}
	)

	.add<lsp::notifications::TextDocument_DidOpen>(
		[state](lsp::notifications::TextDocument_DidOpen::Params&& params)
		{
			const std::string path = uriToPath(params.textDocument.uri.toString());
			state->documents[path] = params.textDocument.text;
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
					
					const LSPData data = provideLSPCache(path);
					state->applyLSPData(data);

					// Text aus Cache holen statt Datei öffnen
					const auto& text = state->documents[path];

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

					for(const auto& [key, details] : data.staticVariables)
						if(key.first == word)
							hoverContent += "- 📦 **variable** `" + details.second + "`\n";

					for(const auto& [key, details] : data.memberVariables)
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

	.add<lsp::requests::TextDocument_Definition>(
		[state](lsp::requests::TextDocument_Definition::Params&& params)
		{
			return std::async(std::launch::deferred,
				[state, params = std::move(params)]()
				{
					const std::string path = uriToPath(params.textDocument.uri.toString());
					const auto line = params.position.line;
					const auto col  = params.position.character;

					std::cerr << "[GotoDefinition] triggered at " << path << ":" << (line+1) << ":" << (col+1) << std::endl;

					//
					const auto& text = state->documents[path];

					size_t lineStart = 0;
					for(uint32_t i = 0; i < line; i++){
						lineStart = text.find('\n', lineStart);
						if(lineStart == std::string::npos) break;
						lineStart++;
					}

					// // können Operatoren von Wörtern absplitten >> Trennung durch Space nicht nötig, Operatoren nicht behandel bar
					// auto isWordChar = [](char c) {
					// 	return std::isalnum(c) || c == '_';
					// };

					// können Operatoren nicht von Wörtern absplitten >> Trennung durch Space nötig
					auto isWordChar = [](char c) {
						return !std::isspace(c) && c != '(' && c != ')' && c != '{' 
							&& c != '}' && c != '[' && c != ']' && c != ',' && c != ';';
					};

					size_t wordStart = lineStart + col;
					while(wordStart > 0 && isWordChar(text[wordStart-1]))
						wordStart--;
					size_t wordEnd = lineStart + col;
					while(wordEnd < text.size() && isWordChar(text[wordEnd]))
						wordEnd++;

					const auto word = text.substr(wordStart, wordEnd - wordStart);
					std::cerr << "[GotoDefinition] word: " << word << std::endl;

					//
					const LSPData data = provideLSPCache(path);

					//
					std::vector<Definition> defs = {};

					//
					if(data.definitions.contains(word)){

						auto [begin, end] = data.definitions.equal_range(word);
						for(auto it = begin; it != end; ++it){

							const auto& label = it->first;
							const auto& defi = it->second;

							std::cerr << "[GotoDefinition] " << defi.script << ":" << defi.defiTokenRow << ":" << defi.defiTokenCol << ":" << defi.defiTokenLen << std::endl;

							if(!state->documents.contains(defi.script)){
								std::ifstream file(defi.script);
								state->documents[defi.script] = std::string(
									std::istreambuf_iterator<char>(file),
									std::istreambuf_iterator<char>()
								);
							}

							if(subwordAt(state->documents[defi.script], defi.defiTokenRow - 1, defi.defiTokenCol - 1, defi.defiTokenLen) != defi.label){

								std::cerr << "[GotoDefinition] Definition " << defi.definitionLine << " nicht mehr an erwarteter Position" << std::endl;
								continue;
							}

							defs.emplace_back(defi);
						}
					}

					//
					if(defs.empty()){

						//
						return lsp::requests::TextDocument_Definition::Result{};
					}

					//
					// std::reverse(defs.begin(), defs.end());
					Definition* targetDefi = &defs[0];

					for(size_t i = 0; i < defs.size(); i++){

						// Wenn Cursor bereits auf letzter Defi ist bleibt die aktuelle einfach die start defi
						if(line + 1 == defs[i].defiTokenRow && i < defs.size() - 1){
							
							targetDefi = &defs[i + 1];
						}
					}

					std::cerr << "[GotoDefinition] Target Defi " << getLocationString(*targetDefi) << std::endl;

					// targetDefi Location
					lsp::Location loc{
						.uri   = lsp::Uri::parse(pathToUri(targetDefi->script)),
						.range = {
							.start = { .line = (int32_t)targetDefi->defiTokenRow - 1, .character = (int32_t)targetDefi->defiTokenCol - 1 },
							.end   = { .line = (int32_t)targetDefi->defiTokenRow - 1, .character = (int32_t)(targetDefi->defiTokenCol + targetDefi->defiTokenLen) - 1 }
						}
					};

					// Alle Locations der gefundenen validen Defis
					std::vector<lsp::Location> locations;
					for(const auto& defi : defs){
						locations.push_back(lsp::Location{
							.uri   = lsp::Uri::parse(pathToUri(defi.script)),
							.range = {
								.start = { .line = (uint32_t)defi.defiTokenRow - 1, .character = (uint32_t)defi.defiTokenCol - 1 },
								.end   = { .line = (uint32_t)defi.defiTokenRow - 1, .character = (uint32_t)(defi.defiTokenCol + defi.defiTokenLen) - 1 }
							}
						});
					}

					bool returnTargetDefi = true;

					if(returnTargetDefi){ return lsp::requests::TextDocument_Definition::Result{loc}; }

					return lsp::requests::TextDocument_Definition::Result{locations};
				}
			);
		}
	)

	.add<lsp::requests::TextDocument_References>(
		[state](lsp::requests::TextDocument_References::Params&& params)
		{
			return std::async(std::launch::deferred,
				[state, params = std::move(params)]()
				{
					const std::string path = uriToPath(params.textDocument.uri.toString());
					const auto line = params.position.line;
					const auto col  = params.position.character;

					std::cerr << "[GotoDefinition] triggered at " << path << ":" << (line+1) << ":" << (col+1) << std::endl;

					//
					const auto& text = state->documents[path];

					size_t lineStart = 0;
					for(uint32_t i = 0; i < line; i++){
						lineStart = text.find('\n', lineStart);
						if(lineStart == std::string::npos) break;
						lineStart++;
					}

					// // können Operatoren von Wörtern absplitten >> Trennung durch Space nicht nötig, Operatoren nicht behandel bar
					// auto isWordChar = [](char c) {
					// 	return std::isalnum(c) || c == '_';
					// };

					// können Operatoren nicht von Wörtern absplitten >> Trennung durch Space nötig
					auto isWordChar = [](char c) {
						return !std::isspace(c) && c != '(' && c != ')' && c != '{' 
							&& c != '}' && c != '[' && c != ']' && c != ',' && c != ';';
					};

					size_t wordStart = lineStart + col;
					while(wordStart > 0 && isWordChar(text[wordStart-1]))
						wordStart--;
					size_t wordEnd = lineStart + col;
					while(wordEnd < text.size() && isWordChar(text[wordEnd]))
						wordEnd++;

					const auto word = text.substr(wordStart, wordEnd - wordStart);
					std::cerr << "[GotoDefinition] word: " << word << std::endl;

					//
					const LSPData data = provideLSPCache(path);

					//
					std::vector<Definition> defs = {};

					//
					if(data.definitions.contains(word)){

						auto [begin, end] = data.definitions.equal_range(word);
						for(auto it = begin; it != end; ++it){

							const auto& label = it->first;
							const auto& defi = it->second;

							std::cerr << "[GotoDefinition] " << defi.script << ":" << defi.defiTokenRow << ":" << defi.defiTokenCol << ":" << defi.defiTokenLen << std::endl;

							if(!state->documents.contains(defi.script)){
								std::ifstream file(defi.script);
								state->documents[defi.script] = std::string(
									std::istreambuf_iterator<char>(file),
									std::istreambuf_iterator<char>()
								);
							}

							if(subwordAt(state->documents[defi.script], defi.defiTokenRow - 1, defi.defiTokenCol - 1, defi.defiTokenLen) != defi.label){

								std::cerr << "[GotoDefinition] Definition " << defi.definitionLine << " nicht mehr an erwarteter Position" << std::endl;
								continue;
							}

							defs.emplace_back(defi);
						}
					}

					//
					if(defs.empty()){

						//
						return lsp::requests::TextDocument_References::Result{};
					}

					//
					// std::reverse(defs.begin(), defs.end());
					Definition* targetDefi = &defs[0];

					for(size_t i = 0; i < defs.size(); i++){

						// Wenn Cursor bereits auf letzter Defi ist bleibt die aktuelle einfach die start defi
						if(line + 1 == defs[i].defiTokenRow && i < defs.size() - 1){
							
							targetDefi = &defs[i + 1];
						}
					}

					std::cerr << "[GotoDefinition] Target Defi " << getLocationString(*targetDefi) << std::endl;

					// targetDefi Location
					lsp::Location loc{
						.uri   = lsp::Uri::parse(pathToUri(targetDefi->script)),
						.range = {
							.start = { .line = (int32_t)targetDefi->defiTokenRow - 1, .character = (int32_t)targetDefi->defiTokenCol - 1 },
							.end   = { .line = (int32_t)targetDefi->defiTokenRow - 1, .character = (int32_t)(targetDefi->defiTokenCol + targetDefi->defiTokenLen) - 1 }
						}
					};

					// Alle Locations der gefundenen validen Defis
					std::vector<lsp::Location> locations;
					for(const auto& defi : defs){
						locations.push_back(lsp::Location{
							.uri   = lsp::Uri::parse(pathToUri(defi.script)),
							.range = {
								.start = { .line = (uint32_t)defi.defiTokenRow - 1, .character = (uint32_t)defi.defiTokenCol - 1 },
								.end   = { .line = (uint32_t)defi.defiTokenRow - 1, .character = (uint32_t)(defi.defiTokenCol + defi.defiTokenLen) - 1 }
							}
						});
					}

					bool returnTargetDefi = false;

					// if(returnTargetDefi){ return lsp::requests::TextDocument_References::Result{loc}; }

					return lsp::requests::TextDocument_References::Result{locations};
				}
			);
		}
	)

	.add<lsp::requests::TextDocument_SemanticTokens_Full>(
		[state](lsp::requests::TextDocument_SemanticTokens_Full::Params&& params)
		{
			return std::async(std::launch::deferred,
				[state, params = std::move(params)]()
				{
					const std::string path = uriToPath(params.textDocument.uri.toString());
					const LSPData data = provideLSPCache(path);
					const auto& text = state->documents[path];

					//
					std::cerr << "dyn Syntaxhighlighting Abfrage" << std::endl;

					// Token Typ Indizes, müssen unbedingt mit Angabe die beim Aktivieren des Providers gemacht worden ist übereinstimmen 
					constexpr uint32_t T_KEYWORD  = 0;
					constexpr uint32_t T_TYPE     = 1;
					constexpr uint32_t T_ENUM     = 2;
					constexpr uint32_t T_FUNCTION = 3;
					constexpr uint32_t T_VARIABLE = 4;
					constexpr uint32_t T_OPERATOR = 5;

					struct RawToken {
						uint32_t line, col, len, type;
					};

					std::vector<RawToken> tokens;

					// Bereiche die ignoriert werden sollen (Kommentarzeilen und Strings)
					std::vector<std::pair<size_t, size_t>> ignoredRanges;

					size_t i = 0;
					while(i < text.size()){
						
						// Zeilenkommentar
						if(i + 1 < text.size() && text[i] == '/' && text[i+1] == '/'){

							size_t start = i;

							while(i < text.size() && text[i] != '\n') i++;
							ignoredRanges.push_back({start, i});
						}

						// String
						else if(text[i] == '"'){

							size_t start = i++;

							while(i < text.size() && text[i] != '"'){

								if(text[i] == '\\') i++; // escape überspringen
								i++;
							}

							ignoredRanges.push_back({start, i + 1});
							i++;
						}
						else{
							i++;
						}
					}

					auto isIgnored = [&](size_t pos) -> bool {

						for(const auto& [start, end] : ignoredRanges){

							if(pos >= start && pos < end) return true;
						}
						
						return false;
					};

					// Lambdafunc die alle Vorkommen eines Wortes im Text findet
					auto findAll = [&](const std::string& word, uint32_t tokenType) {

						size_t pos = 0;

						while ((pos = text.find(word, pos)) != std::string::npos) {

							// check ob wort vollständig ist
							bool leftOk  = pos == 0 || !std::isalnum(text[pos-1]) && text[pos-1] != '_';
							bool rightOk = pos + word.size() >= text.size()
										|| !std::isalnum(text[pos + word.size()]) && text[pos + word.size()] != '_';

							if (leftOk && rightOk && !isIgnored(pos)) {

								// Zeile und Spalte berechnen
								uint32_t line = 0, col = 0;

								for (size_t i = 0; i < pos; i++) {
									if (text[i] == '\n') { line++; col = 0; }
									else col++;
								}
								tokens.push_back({ line, col, (uint32_t)word.size(), tokenType });
							}
							pos += word.size();
						}
					};

					// Herausfiltern der Tokenvorkommen
					for (const auto& kw : state->keywords)       		findAll(kw, T_KEYWORD);
					for (const auto& t  : state->typeKeywords)   		findAll(t,  T_TYPE);
					for (const auto& [key, _]  : data.constKeywords)   	findAll(key.first,  T_ENUM);
					for (const auto& [key, _] : data.functions)  		findAll(key.first, T_FUNCTION);
					for (const auto& [key, _] : data.variables)  		findAll(key.first, T_VARIABLE);
					for (const auto& [key, _] : data.staticVariables)  	findAll(key.first, T_VARIABLE);
					for (const auto& [key, _] : data.memberVariables)  	findAll(key.first, T_VARIABLE);
					for (const auto& op : data.operators)        		findAll(op, T_OPERATOR);

					// Funktionstokens rausfiltern die nicht vor einer Klammer stehen
					tokens.erase(
						std::remove_if(tokens.begin(), tokens.end(), [&](const RawToken& tok) {
							
							if(tok.type != T_FUNCTION) return false;

							// Position nach dem Token im Text finden
							// dafür muss die absolute Position aus line/col zurückgerechnet werden
							size_t absPos = 0;
							uint32_t line = 0;
							while(absPos < text.size() && line < tok.line){
								if(text[absPos] == '\n') line++;
								absPos++;
							}
							absPos += tok.col + tok.len;

							// nächstes nicht-leerzeichen suchen
							while(absPos < text.size() && std::isspace(text[absPos])) absPos++;

							return absPos >= text.size() || text[absPos] != '(';
						}),
						tokens.end()
					);

					// Sortiert nach Position und innerhalb der Position nach type
					// >> kleinerer Type wird immer nach vorne sortiert
					std::sort(tokens.begin(), tokens.end(), [](const RawToken& a, const RawToken& b) {
						if (a.line != b.line) return a.line < b.line;
						if (a.col  != b.col)  return a.col  < b.col;
						return a.type < b.type; // niedrigerer Typ-Index gewinnt
					});

					// Duplikate an gleicher Position entfernen
					// durch sortierung fallen die Tokens mit häherem Type bei kollision immer raus
					tokens.erase(
						std::unique(tokens.begin(), tokens.end(), [](const RawToken& a, const RawToken& b) {
							return a.line == b.line && a.col == b.col;
						}),
						tokens.end()
					);
					
					// Delta-Encoding, relative Positionen für LSP Return angeben
					std::vector<uint32_t> encoded;
					encoded.reserve(tokens.size() * 5);

					//
					uint32_t prevLine = 0, prevCol = 0;
					for (const auto& tok : tokens) {

						uint32_t deltaLine = tok.line - prevLine;
						uint32_t deltaCol  = deltaLine == 0 ? tok.col - prevCol : tok.col;

						encoded.push_back(deltaLine);
						encoded.push_back(deltaCol);
						encoded.push_back(tok.len);
						encoded.push_back(tok.type);
						encoded.push_back(0); // modifier

						prevLine = tok.line;
						prevCol  = tok.col;
					}

					lsp::Array<uint32_t> lspData(encoded.begin(), encoded.end());

					return lsp::requests::TextDocument_SemanticTokens_Full::Result(
						lsp::SemanticTokens{ .data = lspData }
					);
				}
			);
		}
	)

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