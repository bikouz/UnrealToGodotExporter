# Godot Exporter

Plugin éditeur Unreal Engine 5.7 qui exporte automatiquement des assets Unreal vers un projet **Godot 4**.

## Ce que ça fait

Deux modes :

- **Tools → Godot Exporter** : écrit dans le projet Godot, sous `assets/export/meshes|anims|levels|materials|textures|prefabs`.
- **Clic droit → Export to Godot** : choisit un dossier, crée `assets/export/` avec ces sous-dossiers (à glisser dans Godot). Pas de `project.godot`. Seuls le mesh choisi et ses matériaux / textures **assignés** sont exportés.

| Asset Unreal | Sortie Godot |
|---|---|
| Static Mesh, Skeletal Mesh | un `.glb` (géométrie) + un `.tscn` dans `prefabs/` (instance du mesh) |
| Level (`.umap`) | `.glb` dans `levels/` (scène glTF : meshes placés, lumières, caméras) |
| AnimSequence (clic droit sur une seule anim) | un `.glb` d’animation, sans projet Godot (le menu reste « Export to Godot ») |
| Texture2D | PNG (ou fichier source si encore disponible sur le disque) |
| Material / Material Instance | `StandardMaterial3D` `.tres` |
| SoundWave | WAV (ou OGG/MP3 original) |
| DataTable | JSON |

Conversions appliquées :

- Unreal (centimètres) → Godot / glTF (mètres), échelle `0.01`
- Flip de la green channel des normal maps (DirectX → OpenGL / Godot)

**Non convertis** (trop spécifiques à Unreal) : Blueprints, Niagara, Control Rig, physique avancée.

Un `project.godot` et un `godot_export_manifest.json` sont écrits **uniquement** depuis **Tools → Godot Exporter**.

## Comment l’utiliser

### Projet Godot (Tools → Godot Exporter)

1. Menu **Tools → Godot Exporter**.
2. Choisis le dossier du projet Godot.
3. Lance **Export All**, **Export Selected Assets** ou **Export Selected Folders**.

### Fichiers seuls (clic droit)

1. Clic droit → **Export to Godot**.
2. Choisis un dossier (Downloads, Bureau, etc.).
3. Un sous-dossier `assets/export/` est créé :

```text
assets/export/
  meshes/      *.glb (static / skeletal, textures en référence, pas de .bin)
  anims/       *.glb (AnimSequence)
  levels/      *.glb (maps / .umap)
  materials/   *.tres
  textures/    *.png
  prefabs/     *.tscn (instance du .glb)
  audio/       *.wav / *.ogg
  data/        *.json
```

4. Glisse le dossier `assets` à la racine du projet Godot (ou exporte directement dans le projet). Les chemins `res://assets/export/...` restent valides.

Un mesh n’embarque **pas** les PNG (ça alourdirait le fichier). Le `.glb` contient la géométrie et **pointe** vers `textures/`, donc le preview Godot les affiche. Un seul fichier à déplacer (plus de `.bin` orphelin). La scène utilise les `.tres` (`use_external`).

Les maps (`.umap` → `levels/`) restent un `.glb` de scène (lumières, placements) avec PBR baké.

### Ligne de commande

```text
UnrealEditor-Cmd.exe Downgrader.uproject -run=GodotExport -GodotProject="C:/Godot/MonJeu" -Path=/Game
```

- `-GodotProject` : dossier du projet Godot
- `-Path` : chemin Content Unreal à exporter (défaut : `/Game`)

### Blueprint / Editor Utility

Les fonctions sont dans la catégorie **Godot Export** :

- `Export Asset To Godot`
- `Export Assets To Godot`
- `Export Content Folder To Godot`
- `Export All To Godot`

## Réglages utiles

| Option | Rôle |
|---|---|
| Godot Project Folder | Dossier du projet Godot (Tools → Godot Exporter) |
| Export Subfolder | Sous-dossier créé à la destination (`assets/export` par défaut) |
| Source Content Path | Racine Unreal pour Export All (`/Game` par défaut) |
| Use Binary Glb | `.glb` (recommandé) ou `.gltf` + sidecars |
| Include Dependencies | Pour un mesh / matériau / level, exporte aussi les textures et matériaux référencés. Une AnimSequence n’exporte que l’animation. |
| Skip Unchanged | Ignore un asset si le fichier Godot est déjà plus récent |
| Auto Export On Save | Réexporte un asset supporté dès qu’il est sauvegardé |
| Prefer Original Source Files | Copie le PNG/WAV/OGG d’import s’il existe encore |
| Export Prefabs | Écrit un `.tscn` dans `prefabs/` pour chaque mesh |

## Dépendance

Le plugin Unreal **glTF Exporter** (`GLTFExporter`) doit rester activé : les meshes, animations et levels passent par lui.
