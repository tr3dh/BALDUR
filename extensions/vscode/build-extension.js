const esbuild = require('esbuild');

esbuild.build({
    entryPoints: ['extension.js'],
    bundle: true,
    outfile: 'dist/extension.js',
    external: ['vscode'],
    format: 'cjs',
    platform: 'node',
    target: 'node16',
    minify: false,  // Erstmal false für debugging
    sourcemap: false
}).then(() => {
    console.log('✓ Extension gebundelt!');
}).catch((err) => {
    console.error('✗ Build-Fehler:', err);
    process.exit(1);
});