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
import * as fs from 'fs'
import * as path from 'path'

/**
 * Handles changes in configurations files.
 * Invokes configuration prompts in case they are missing.
 * Stores and checks wildcards for file matching.
 */
export class ConfigurationsHandler {
    // defined regex expression to match files and recognize them as HLASM
    private definedExpressions: RegExp[];
    // paths to the configurations
    private pgmConfPath: string;
    private procGrpsPath: string;
    private folderPath: string;
    // whether to create warning prompts on missing configs
    shouldCheckConfigs: boolean;

    constructor() {
        this.definedExpressions = [];
        this.pgmConfPath = undefined;
        this.procGrpsPath = undefined;
        this.folderPath = undefined;
        this.shouldCheckConfigs = true;
    }

    /**
     * Checks whether the given path matches any of the wildcards
     * If so, it is HLASM
     */
    match(file: string): boolean {
        return this.definedExpressions.find(expr => expr.test(file)) !== undefined;
    }

    /**
     * Checks whether both config files are present
     * Creates them on demand if not
     */
    checkConfigs(): [string, string] {
        // configs exist
        if (this.updateConfigPaths())
            return [this.pgmConfPath, this.procGrpsPath];

        // no workspace
        if (!vscode.workspace.workspaceFolders)
            return ['', ''];

        const doNotShowAgain = 'Do not track';
        // give option to create proc_grps
        if (!fs.existsSync(this.procGrpsPath)) {
            vscode.window.showWarningMessage('proc_grps.json not found',
                ...['Create empty proc_grps.json', doNotShowAgain])
                .then((selection) => {
                    if (selection) {
                        if (selection == doNotShowAgain) {
                            this.shouldCheckConfigs = false;
                            return;
                        }
                        this.createProcTemplate();
                    }
                });
        }
        if (!fs.existsSync(this.pgmConfPath)) {
            vscode.window.showWarningMessage('pgm_conf.json not found',
                ...['Create empty pgm_conf.json', 'Create pgm_conf.json with this file', doNotShowAgain])
                .then((selection) => {
                    if (selection) {
                        if (selection == doNotShowAgain) {
                            this.shouldCheckConfigs = false;
                            return;
                        }
                        this.createPgmTemplate(selection == 'Create empty pgm_conf.json');
                    }
                });
        }
        return [this.pgmConfPath, this.procGrpsPath];
    }

    // update wildcards when pgm conf changes (on save)
    updateWildcards(reloadedFile: string = undefined): RegExp[] {
        // the reloaded file is not a config file
        if (reloadedFile && reloadedFile != this.pgmConfPath && reloadedFile != this.procGrpsPath)
            return this.definedExpressions;
        // configs do not exist
        if (!this.updateConfigPaths())
            return [];

        //clear expressions
        this.definedExpressions = [];
        // get user-defined wildcards
        var content = JSON.parse(fs.readFileSync(this.pgmConfPath, "utf8"));

        // convert each pgm to regex
        if (content.pgms) {
            (content.pgms as any[]).forEach(pgm => {
                const regex = this.convertWildcardToRegex(path.join(vscode.workspace.workspaceFolders[0].uri.fsPath, pgm.program as string));
                if (regex)
                    this.definedExpressions.push(regex);
            });
        }

        // convert each wildcard to regex
        if (content.alwaysRecognize) {
            (content.alwaysRecognize as string[]).forEach(strExpr => {
                const regex = this.convertWildcardToRegex(path.join(vscode.workspace.workspaceFolders[0].uri.fsPath, strExpr));
                if (regex)
                    this.definedExpressions.push(regex);
            });
        }

        content = JSON.parse(fs.readFileSync(this.procGrpsPath, "utf8"));
        // convert each pgroup library path to regex
        if (content.pgroups) {
            (content.pgroups as any[]).forEach(pgroup => {
                if (pgroup.libs)
                    (pgroup.libs as any[]).forEach(lib => {
                        const regex = this.convertWildcardToRegex(path.join(vscode.workspace.workspaceFolders[0].uri.fsPath, lib.path || lib, '*'));
                        if (regex)
                            this.definedExpressions.push(regex);
                    })
            });
        }
        return this.definedExpressions;
    }

    private createPgmTemplate(empty: boolean) {
        if (!fs.existsSync(this.folderPath))
            fs.mkdirSync(this.folderPath);

        var programName = '';
        if (!empty)
            programName = vscode.window.activeTextEditor.document.fileName.split('\\').pop().split('/').pop();
        fs.writeFileSync(this.pgmConfPath, JSON.stringify(
            { "pgms": [{ "program": programName, "pgroup": "" }], "alwaysRecognize": [] }
            , null, 2));
        vscode.commands.executeCommand("vscode.open", vscode.Uri.file(this.pgmConfPath));
    }

    private createProcTemplate() {
        if (!fs.existsSync(this.folderPath))
            fs.mkdirSync(this.folderPath);

        fs.writeFile(this.procGrpsPath, JSON.stringify(
            { "pgroups": [{ "name": "", "libs": [""] }] }
            , null, 2), () => { });
        vscode.commands.executeCommand("vscode.open", vscode.Uri.file(this.procGrpsPath));
    }

    // converts wildcards to regexes
    private convertWildcardToRegex(wildcard: string): RegExp {
        var regexStr = wildcard.replace(/\(|\[|\{|\\|\^|\-|\=|\$|\!|\||\]|\}|\)|\./g, (char) => { return "\\" + char });
        regexStr = regexStr.replace(/\?/g, ".");
        regexStr = regexStr.replace(/\*|\+/g, (char) => { return "." + char + "?"; });
        regexStr = '^' + regexStr + '$';
        return new RegExp(regexStr);
    }


    /**
     * Checks if the configs are there and stores their complete paths
     */
    private updateConfigPaths(): boolean {
        // paths are defined and existing
        if (this.procGrpsPath && this.pgmConfPath && fs.existsSync(this.procGrpsPath) && fs.existsSync(this.pgmConfPath))
            return true;
        // no workspace, no configs
        if (!vscode.workspace.workspaceFolders)
            return false;

        // update .hlasmplugin folder
        const folder = vscode.workspace.workspaceFolders[0].uri.fsPath;
        this.folderPath = path.join(folder, '.hlasmplugin');

        // paths where the configs are supposed to be
        this.procGrpsPath = path.join(this.folderPath, 'proc_grps.json');
        this.pgmConfPath = path.join(this.folderPath, 'pgm_conf.json');

        if (!fs.existsSync(this.procGrpsPath))
            return false;

        if (!fs.existsSync(this.pgmConfPath)) {
            // file might have been removed, clear the expressions
            this.definedExpressions = [];
            return false;
        }

        return true;
    }
}
