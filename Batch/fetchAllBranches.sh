#!/bin/bash

# in Git bash aufrufen mit 
# bash Batch/fetchAllBranches.sh

# Alle Remote-Branches automatisch lokal einrichten und veraltete/umbenannte Branches erkennen und aufräumen
 
if ! git rev-parse --git-dir > /dev/null 2>&1; then
    echo "Fehler: Kein Git-Repository gefunden"
    exit 1
fi

echo "=== Fetch origin mit --prune flag ==="
git fetch --prune origin

echo ""
echo "=== Lokale Branches für alle Remote-Branches anlegen ==="
for remote in $(git branch -r | grep -v '\->'); do
    local_branch="${remote#origin/}"
    if ! git show-ref --verify --quiet "refs/heads/$local_branch"; then
        echo "Erstelle $local_branch mit Tracking zu $remote"
        git branch --track "$local_branch" "$remote"
    else
        echo "✓ $local_branch exists bereits"
    fi
done

echo ""
echo "=== Verwaiste lokale Branches (kein Remote mehr) ==="
orphan_found=false
for local in $(git branch | sed 's/[\*+]//;s/ //g'); do
    if ! git show-ref --verify --quiet "refs/remotes/origin/$local"; then
        echo "⚠️  '$local' hat keinen Remote-Branch mehr"
        echo "   → Löschen mit: git branch -d $local"
        echo "   → Erzwungen:   git branch -D $local"
        orphan_found=true
    fi
done

if [ "$orphan_found" = false ]; then
    echo "✓ Alle lokalen Branches haben einen Remote-Branch"
fi
 
echo ""
echo "=== Pull alle Remotes ==="
git pull --all 2>/dev/null || echo "(nur bei mehreren Remotes relevant)"
 
echo ""
echo "=== Fertig! Aktueller Branch-Status ==="
git branch -vv