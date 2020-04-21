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

import * as path from 'path';
import * as Mocha from 'mocha';
import * as glob from 'glob';
import * as vscode from 'vscode';

export async function run(): Promise<void> {
	const NYC = require('nyc');
	const nyc = new NYC({ 
		cwd: path.join(__dirname, '..', '..','..'),
		exclude: ['.vscode-test/**'],
		reporter: ['lcov'],
		hookRequire: true,
		extension: [".ts"],
		instrument: true,
		cache: false
	});

	nyc.createTempDirectory();
	nyc.wrap();

	// Create the mocha test
	const mocha = new Mocha({ ui: 'tdd', color: true });
	const testsPath = path.join(__dirname, '..');

	await new Promise((resolve, reject) => {
		glob('**/**.test.js', { cwd: testsPath }, (_, files) => {
				// Add files to the test suite
				files.forEach(file => 
					mocha.addFile(path.resolve(testsPath, file)));
	
				try {
					vscode.workspace.getConfiguration('hlasmplugin').update('continuationHandling',true).then(() => {
						// Run the mocha test
						mocha.run(failures => {
							if (failures > 0) {
								reject(new Error(`${failures} tests failed.`));
							} else {
								resolve();
							}
						});
					});
				} catch (error) {
					console.error(error);
					reject(error);
				}
		});
	});

	nyc.writeCoverageFile();
	nyc.report();
	console.log('Report created');
}