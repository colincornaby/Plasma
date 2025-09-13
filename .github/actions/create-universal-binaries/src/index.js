import * as core from "@actions/core";
import * as github from "@actions/github";
import * as exec from "@actions/exec";
import * as fs from "fs";
const fsp = fs.promises;
import * as path from "path";

async function copyTree(src, dest) {
	await fsp.mkdir(dest, { recursive: true });
	const entries = await fsp.readdir(src, { withFileTypes: true });

	for (const entry of entries) {
		const srcPath = path.join(src, entry.name);
		const destPath = path.join(dest, entry.name);

		if (entry.isSymbolicLink()) {
			const linkTarget = await fsp.readlink(srcPath);
			await fsp.symlink(linkTarget, destPath);
		} else if (entry.isDirectory()) {
			await copyTree(srcPath, destPath);
		} else if (entry.isFile()) {
			await fsp.copyFile(srcPath, destPath);
		}
	}
}

function isExecutable(filePath) {
	if (!fs.existsSync(filePath) || !fs.statSync(filePath).isFile()) {
		return false;
	}
	try {
		const output = execSync(`file "${filePath}"`, { encoding: 'utf8' });
		return output.includes('Mach-O') && output.includes('executable');
	} catch {
		return false;
	}
}

async function mergeExecutables(folder1, folder2, output) {
	const walk = async (dir) => {
		const entries = await fsp.readdir(dir, { withFileTypes: true });
		for (const entry of entries) {
			const outPath = path.join(dir, entry.name);
			const relPath = path.relative(output, outPath);
			const otherPath = path.join(folder2, relPath);

			if (entry.isDirectory()) {
				await walk(outPath);
			} else if (entry.isFile()) {
				if (isExecutable(outPath) && fs.existsSync(otherPath) && isExecutable(otherPath)) {
					core.info(`Merging ${relPath}`);
					await exec.exec('lipo', ['-create', '-output', outPath, outPath, otherPath]);
				}
			}
		}
	};
	await walk(output);
}

async function run() {
	try {
		const folder1 = core.getInput('folder1');
		const folder2 = core.getInput('folder2');
		const output = core.getInput('output');

		if (!fs.existsSync(folder1) || !fs.existsSync(folder2)) {
			core.setFailed('Both folder1 and folder2 must exist.');
			return;
		}
		if (fs.existsSync(output)) {
			core.setFailed(`Output folder ${output} already exists.`);
			return;
		}

		// Step 1: Copy folder1 into output
		await copyTree(folder1, output);

		// Step 2: Walk output and merge executables with lipo
		await mergeExecutables(folder1, folder2, output);

		core.info('Universal binary merge complete.');
	} catch (error) {
		core.setFailed(error.message);
	}
}

run();