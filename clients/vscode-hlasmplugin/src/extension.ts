/*
 * Copyright (c) 2019 Broadcom.
 * The term "Broadcom" refers to Broadcom Inc. and/or its subsidiaries.
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Broadcom, Inc. - initial API and implementation
 */

import * as vscode from 'vscode';
import * as vscodelc from 'vscode-languageclient';
import * as path from 'path'

import { HLASMSemanticHighlightingFeature } from './hlasmSemanticHighlighting'
import { HlasmConfigurationProvider, getCurrentProgramName, getProgramName } from './debug'
import { ContinuationHandler } from './continuationHandler'
import { CustomEditorCommands } from './customEditorCommands'
import { EventsHandler, getConfig } from './eventsHandler'
import { ServerFactory } from './serverFactory'
const useTcp = false;

/**
 * ACTIVATION
 * activates the extension
 */
export async function activate(context: vscode.ExtensionContext) {
    // patterns for files and configs
    const filePattern: string = '**/*'
    const configPattern: string =
        '**/{'
        + [path.join('.hlasmplugin', 'proc_grps.json'), path.join('.hlasmplugin', 'pgm_conf.json')].join()
        + '}';
    
    // create client options
    const syncFileEvents = getConfig<boolean>('syncFileEvents', true);
    const clientOptions: vscodelc.LanguageClientOptions = {
        documentSelector: [{ language: 'hlasm' }, { pattern: configPattern }],
        synchronize: !syncFileEvents ? undefined : {
            fileEvents: vscode.workspace.createFileSystemWatcher(filePattern)
        },
        uriConverters: {
            code2Protocol: (value: vscode.Uri) => value.toString(),
            protocol2Code: (value: string) =>
                vscode.Uri.file((vscode.Uri.parse(value).fsPath))
        }
    };
    
    // create server options
    var factory = new ServerFactory();
    const serverOptions = await factory.create(useTcp);

    //client init
    const hlasmpluginClient = new vscodelc.LanguageClient('Hlasmplugin Language Server', serverOptions, clientOptions);
    //asm contribution 
    var highlight = new HLASMSemanticHighlightingFeature(hlasmpluginClient);

    // register highlighting as features
    hlasmpluginClient.registerFeature(highlight);

    // register all commands and objects to context
    registerToContext(context, factory.dapPort, highlight);

    //give the server some time to start listening when using TCP
    setTimeout(function () {
        hlasmpluginClient.start();
    }, (useTcp) ? 2000 : 0);
}

async function registerToContext(context: vscode.ExtensionContext, dapPort: number, highlight: HLASMSemanticHighlightingFeature) {
    // vscode/theia compatibility temporary fix
    // theia uses monaco commands
    var completeCommand: string;
    await vscode.commands.getCommands().then((result) => {
        if (result.find(command => command == "editor.action.triggerSuggest"))
            completeCommand = "editor.action.triggerSuggest";
        else if (result.find(command => command == "monaco.editor.action.triggerSuggest"))
            completeCommand = "monaco.editor.action.triggerSuggest";
    });
    // initialize helpers
    const contHandling = new ContinuationHandler(highlight);
    const commands = new CustomEditorCommands(highlight);
    const handler = new EventsHandler(completeCommand, highlight);

    // register them
    context.subscriptions.push(contHandling);
    context.subscriptions.push(commands);
    context.subscriptions.push(handler);

    // register provider for all hlasm debug configurations
    context.subscriptions.push(vscode.debug.registerDebugConfigurationProvider('hlasm', new HlasmConfigurationProvider(dapPort)));

    // register continuation handlers
    context.subscriptions.push(vscode.commands.registerTextEditorCommand("insertContinuation", 
        (editor, edit) => contHandling.insertContinuation(editor, edit)));
    context.subscriptions.push(vscode.commands.registerTextEditorCommand("removeContinuation", 
        (editor, edit) => contHandling.removeContinuation(editor, edit)));

    // overrides should happen only if the user wishes
    if (getConfig<boolean>('continuationHandling', false)) {
        context.subscriptions.push(vscode.commands.registerTextEditorCommand("type",
            (editor, edit, args) => commands.type(editor, edit, args)));
        context.subscriptions.push(vscode.commands.registerTextEditorCommand("paste", 
            (editor, edit, args) => commands.paste(editor, edit, args)));
        context.subscriptions.push(vscode.commands.registerTextEditorCommand("cut", 
            (editor, edit) => commands.cut(editor, edit)));
        context.subscriptions.push(vscode.commands.registerTextEditorCommand("deleteLeft", 
            (editor, edit) => commands.deleteLeft(editor, edit)));
        context.subscriptions.push(vscode.commands.registerTextEditorCommand("deleteRight", 
            (editor, edit) => commands.deleteRight(editor, edit)));
    }

    // register event handlers
    context.subscriptions.push(vscode.workspace.onDidChangeTextDocument(e => handler.onDidChangeTextDocument(e)));
    context.subscriptions.push(vscode.workspace.onDidCloseTextDocument(e => handler.onDidCloseTextDocument(e)));
    context.subscriptions.push(vscode.workspace.onDidOpenTextDocument(e => handler.onDidOpenTextDocument(e)));
    context.subscriptions.push(vscode.workspace.onDidChangeConfiguration(e => handler.onDidChangeConfiguration(e)));
    context.subscriptions.push(vscode.workspace.onDidSaveTextDocument(e => handler.onDidSaveTextDocument(e)));
    context.subscriptions.push(vscode.window.onDidChangeVisibleTextEditors(e => handler.onDidChangeVisibleTextEditors(e)));
    context.subscriptions.push(vscode.window.onDidChangeActiveTextEditor(e => handler.onDidChangeActiveTextEditor(e)));

    // register highlight progress
    context.subscriptions.push(highlight.progress);

    // register filename retrieve functions for debug sessions
    context.subscriptions.push(vscode.commands.registerCommand('extension.hlasm-plugin.getProgramName', () => getProgramName()));
    context.subscriptions.push(vscode.commands.registerCommand('extension.hlasm-plugin.getCurrentProgramName', () => getCurrentProgramName()));
}