const core = require('@actions/core');
const exec = require('@actions/exec');
const fs = require('fs');
const fsp = fs.promises;
const path = require('path');
const { spawnSync } = require('child_process');

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
  try {
	const stats = fs.statSync(filePath);
	return stats.isFile() && (stats.mode & fs.constants.S_IXUSR);
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
		  const archs1 = getArchs(outPath);
		  const archs2 = getArchs(otherPath);

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