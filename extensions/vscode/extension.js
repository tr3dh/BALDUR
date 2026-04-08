const vscode = require('vscode');
const path = require('path');
const fs = require('fs');
const https = require('https')
const { LanguageClient, TransportKind } = require('vscode-languageclient/node');
const { execFile } = require('child_process'); //

let client;
let terminal;
let outputChannel;
let statusBarItem;

async function getLatestTag(owner, repo) {
    return new Promise((resolve, reject) => {
        const options = {
            hostname: "api.github.com",
            path: `/repos/${owner}/${repo}/releases/latest`,
            headers: {
                "User-Agent": "vscode-extension",
                "Accept": "application/vnd.github.v3+json"
            }
        };

        https.get(options, (res) => {
            let data = "";
            res.on("data", (chunk) => data += chunk);
            res.on("end", () => {
                try {
                    const json = JSON.parse(data);
                    resolve(json.tag_name);
                } catch (err) {
                    reject(err);
                }
            });
        }).on("error", reject);
    });
}

async function activate(context) {

    // Output Channel erstellen
    outputChannel = vscode.window.createOutputChannel('Baldur Extension');
    
    // Status Bar Item erstellen
    statusBarItem = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Right, 100);
    statusBarItem.text = "$(loading~spin) Baldur lädt...";
    statusBarItem.show();
    context.subscriptions.push(statusBarItem);
    
    //
    outputChannel.appendLine('=== Baldur extension activating ===');
    console.log('Baldur extension activating...');
    
    try {
        const tag = await getLatestTag("raysan5", "raylib");
        outputChannel.appendLine(`Latest tag: ${tag}`); // das ist die Stelle, wo du es siehst
    } catch (err) {
        outputChannel.appendLine(`Error: ${err.message}`);
    }

    try {

        // Pfade
        const serverExe = path.join(context.extensionPath, 'build', 'build', 'ProcLSP.exe');
        const langExe   = path.join(context.extensionPath, 'build', 'build', 'ProcLang.exe');

        outputChannel.appendLine(`Extension path: ${context.extensionPath}`);
        outputChannel.appendLine(`Server exe: ${serverExe}`);
        outputChannel.appendLine(`Lang exe: ${langExe}`);
        
        console.log('Extension path:', context.extensionPath);
        console.log('Server exe:', serverExe);
        console.log('Lang exe:', langExe);

        // Prüfe ob Dateien existieren
        let hasErrors = false;
        
        if (!fs.existsSync(serverExe)) {
            outputChannel.appendLine('❌ Server exe NOT FOUND!');
            vscode.window.showErrorMessage(`❌ Language Server nicht gefunden: ${serverExe}`, 'Logs zeigen')
                .then(selection => {
                    if (selection === 'Logs zeigen') {
                        outputChannel.show();
                    }
                });
            console.error('Server exe not found:', serverExe);
            hasErrors = true;
        } else {
            outputChannel.appendLine('✓ Server exe found');
        }
        
        if (!fs.existsSync(langExe)) {
            outputChannel.appendLine('❌ Lang exe NOT FOUND!');
            vscode.window.showErrorMessage(`❌ Baldur Runtime nicht gefunden: ${langExe}`, 'Logs zeigen')
                .then(selection => {
                    if (selection === 'Logs zeigen') {
                        outputChannel.show();
                    }
                });
            console.error('Lang exe not found:', langExe);
            hasErrors = true;
        } else {
            outputChannel.appendLine('✓ Lang exe found');
        }

        if (hasErrors) {
            statusBarItem.text = "$(error) Baldur: Fehler";
            statusBarItem.backgroundColor = new vscode.ThemeColor('statusBarItem.errorBackground');
            statusBarItem.tooltip = "Baldur Extension hat Fehler beim Laden. Klicke für Details.";
            statusBarItem.command = 'workbench.action.output.toggleOutput';
            return;
        }

        // Server Options
        const serverOptions = {
            command: serverExe,
            transport: TransportKind.stdio,
            options: {
                cwd: context.extensionPath
            }
        };

        // Client Options
        const clientOptions = {
            documentSelector: [{ scheme: 'file', language: 'bld' }],
            synchronize: {
                fileEvents: vscode.workspace.createFileSystemWatcher('**/*.bld')
            },
            outputChannelName: 'Baldur Language Server'
        };

        // Run Command
        const runCommand = vscode.commands.registerCommand('bld.runFile', () => {

            outputChannel.appendLine('>>> Running bld.runFile command');
            console.log('Running bld.runFile command');
            
            // Temporäres Status Update
            const originalText = statusBarItem.text;
            statusBarItem.text = "$(play) Baldur läuft...";
            
            const editor = vscode.window.activeTextEditor;
            if (!editor) {
                outputChannel.appendLine('❌ No active editor');
                vscode.window.showWarningMessage("⚠️ Keine aktive .bld Datei zum Ausführen gefunden!");
                statusBarItem.text = originalText;
                return;
            }

            let filePath = editor.document.fileName;
            outputChannel.appendLine(`File to run: ${filePath}`);

            if (filePath[1] === ':') {
                filePath = filePath[0].toUpperCase() + filePath.slice(1);
            }

            if (!terminal || terminal.exitStatus) {
                terminal = vscode.window.createTerminal('Baldur');
                outputChannel.appendLine('Created new terminal');
            }

            const command = `${langExe} execute ${filePath}`;
            outputChannel.appendLine(`Terminal command: ${command}`);
            
            // Notification mit Dateinamen
            const fileName = path.basename(filePath);
            vscode.window.showInformationMessage(`▶️ Führe aus: ${fileName}`);
            
            terminal.show(true);
            terminal.sendText(command);
            
            // Status zurücksetzen nach 2 Sekunden
            setTimeout(() => {
                statusBarItem.text = originalText;
            }, 2000);
        });

        const runButtonCommand = vscode.commands.registerCommand('bld.runFileButton', () => {
            outputChannel.appendLine('>>> Running bld.runFileButton command');
            console.log('Running bld.runFileButton command');
            vscode.commands.executeCommand('bld.runFile');
        });

        context.subscriptions.push(runCommand);
        context.subscriptions.push(runButtonCommand);
        outputChannel.appendLine('✓ Commands registered');
        
        vscode.window.showInformationMessage('✓ Baldur Commands registriert');

        // Debug Provider
        const debugProvider = vscode.debug.registerDebugConfigurationProvider('baldur', {
            resolveDebugConfiguration(folder, config) {
                outputChannel.appendLine('>>> Debug configuration resolved');
                const editor = vscode.window.activeTextEditor;
                if (editor && !config.program) {
                    config.program = editor.document.fileName;
                }
                vscode.commands.executeCommand('bld.runFile');
                return null;
            }
        });
        
        context.subscriptions.push(debugProvider);
        outputChannel.appendLine('✓ Debug provider registered');

        // Language Client
        outputChannel.appendLine('Starting language client...');
        statusBarItem.text = "$(loading~spin) LSP startet...";
        
        client = new LanguageClient(
            'bldLanguageServer',
            'Baldur Language Server',
            serverOptions,
            clientOptions
        );

        client.start().then(() => {
            outputChannel.appendLine('✓ Language client started successfully');
            console.log('Language client started successfully');
            
            // Erfolgreicher Start
            statusBarItem.text = "$(check) Baldur bereit";
            statusBarItem.backgroundColor = undefined;
            statusBarItem.tooltip = "Baldur Language Extension ist aktiv\nF5 zum Ausführen";
            statusBarItem.command = undefined;
            
            vscode.window.showInformationMessage('✅ Baldur Language Extension erfolgreich gestartet!');
        }).catch(err => {
            outputChannel.appendLine(`❌ Language client start error: ${err.message}`);
            console.error('Language client start error:', err);
            
            // Fehler anzeigen
            statusBarItem.text = "$(warning) Baldur: LSP Fehler";
            statusBarItem.backgroundColor = new vscode.ThemeColor('statusBarItem.warningBackground');
            statusBarItem.tooltip = `Language Server Fehler: ${err.message}`;
            statusBarItem.command = 'workbench.action.output.toggleOutput';
            
            vscode.window.showErrorMessage(
                `❌ Language Server konnte nicht gestartet werden: ${err.message}`,
                'Logs zeigen'
            ).then(selection => {
                if (selection === 'Logs zeigen') {
                    outputChannel.show();
                }
            });
        });

        context.subscriptions.push(client);
        context.subscriptions.push(outputChannel);

    } catch (error) {
        outputChannel.appendLine(`❌ Activation error: ${error.message}`);
        outputChannel.appendLine(error.stack);
        console.error('Activation error:', error);
        
        // Kritischer Fehler
        statusBarItem.text = "$(error) Baldur: Fehler";
        statusBarItem.backgroundColor = new vscode.ThemeColor('statusBarItem.errorBackground');
        statusBarItem.tooltip = `Activation Fehler: ${error.message}`;
        statusBarItem.command = 'workbench.action.output.toggleOutput';
        
        vscode.window.showErrorMessage(
            `❌ Extension Activation fehlgeschlagen: ${error.message}`,
            'Logs zeigen',
            'Extension neu laden'
        ).then(selection => {
            if (selection === 'Logs zeigen') {
                outputChannel.show();
            } else if (selection === 'Extension neu laden') {
                vscode.commands.executeCommand('workbench.action.reloadWindow');
            }
        });
    }
}

function deactivate() {
    if (outputChannel) {
        outputChannel.appendLine('=== Deactivating extension ===');
    }
    
    if (statusBarItem) {
        statusBarItem.dispose();
    }
    
    if (terminal) {
        terminal.dispose();
    }
    
    if (!client) {
        return undefined;
    }
    
    return client.stop();
}

module.exports = {
    activate,
    deactivate
};