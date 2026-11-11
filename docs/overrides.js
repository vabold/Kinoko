//////////////////////////////////////////////
// Expand inheritance diagrams by default //
//////////////////////////////////////////////
// Doxygen collapses "Inheritance diagram" dynamic sections by default. Expand
// them immediately on load so the diagram is visible without an extra click.
document.querySelectorAll('.dynheader').forEach(header => {
    if (!header.textContent.trim().startsWith('Inheritance diagram')) {
        return;
    }

    const content = document.getElementById(header.id + '-content');
    if (!content) {
        return;
    }

    header.classList.remove('closed');
    content.classList.remove('hidden');
    header.querySelectorAll('.arrowhead').forEach(arrow => {
        arrow.classList.remove('closed');
        arrow.classList.add('opened');
    });
});

//////////////////////////////
// Highlight diff fragments //
//////////////////////////////
// Doxygen has no diff parser. Detect them by content instead: every non-empty
// line starts with '+', '-', or ' ', and at least one is a change.
document.querySelectorAll('div.fragment').forEach(fragment => {
    const lines = [...fragment.querySelectorAll(':scope > div.line')];
    const nonEmpty = lines.filter(line => line.textContent.length > 0);

    if (!nonEmpty.every(line => /^[+\- ]/.test(line.textContent)) ||
            !nonEmpty.some(line => /^[+-]/.test(line.textContent))) {
        return;
    }

    lines.forEach(line => {
        if (line.textContent.startsWith('+')) {
            line.classList.add('diff-add');
        } else if (line.textContent.startsWith('-')) {
            line.classList.add('diff-del');
        }
    });
});

////////////////////////////
// Remove "More..." Links //
////////////////////////////

document.querySelectorAll('a').forEach(link => {
    if (link.textContent.trim() === 'More...') {
        link.remove();
    }
});
