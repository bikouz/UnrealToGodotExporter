# Godot Exporter

Unreal Engine 5.7 editor plugin that automatically exports Unreal assets to a **Godot 4** project.

## What it does

Two modes:

- **Tools → Godot Exporter**: writes into the Godot project, under `assets/export/meshes|anims|levels|materials|textures|prefabs`.
- **Right-click → Export to Godot**: pick a folder, creates `assets/export/` with those subfolders (drag into Godot). No `project.godot`. Only the selected mesh and its **assigned** materials / textures are exported.

| Unreal asset | Godot output |
|---|---|
| Static Mesh, Skeletal Mesh | a `.glb` (geometry) + a `.tscn` in `prefabs/` (mesh instance) |
| Level (`.umap`) | `.glb` in `levels/` (glTF scene: placed meshes, lights, cameras) |
| AnimSequence (right-click a single anim) | an animation `.glb`, no Godot project (the menu stays “Export to Godot”) |
| Texture2D | PNG (or the source file if it is still on disk) |
| Material / Material Instance | `StandardMaterial3D` `.tres` |
| SoundWave | WAV (or original OGG/MP3) |
| DataTable | JSON |

Applied conversions:

- Unreal (centimeters) → Godot / glTF (meters), scale `0.01`
- Green-channel flip on normal maps (DirectX → OpenGL / Godot)

**Not converted** (too Unreal-specific): Blueprints, Niagara, Control Rig, advanced physics.

A `project.godot` and a `godot_export_manifest.json` are written **only** from **Tools → Godot Exporter**.

## How to use it

### Godot project (Tools → Godot Exporter)

1. Menu **Tools → Godot Exporter**.
2. Pick the Godot project folder.
3. Run **Export All**, **Export Selected Assets**, or **Export Selected Folders**.

### Standalone files (right-click)

1. Right-click → **Export to Godot**.
2. Pick a folder (Downloads, Desktop, etc.).
3. An `assets/export/` subfolder is created:

```text
assets/export/
  meshes/      *.glb (static / skeletal, textures by reference, no .bin)
  anims/       *.glb (AnimSequence)
  levels/      *.glb (maps / .umap)
  materials/   *.tres
  textures/    *.png
  prefabs/     *.tscn (instance of the .glb)
  audio/       *.wav / *.ogg
  data/        *.json
```

4. Drag the `assets` folder to the root of the Godot project (or export directly into the project). Paths `res://assets/export/...` stay valid.

A mesh does **not** embed PNGs (that would bloat the file). The `.glb` holds geometry and **points** at `textures/`, so Godot’s preview shows them. One file to move (no orphan `.bin`). The scene uses `.tres` files (`use_external`).

Maps (`.umap` → `levels/`) remain a scene `.glb` (lights, placements) with baked PBR.

### Command line

```text
UnrealEditor-Cmd.exe Downgrader.uproject -run=GodotExport -GodotProject="C:/Godot/MyGame" -Path=/Game
```

- `-GodotProject`: Godot project folder
- `-Path`: Unreal Content path to export (default: `/Game`)

### Blueprint / Editor Utility

Functions live in the **Godot Export** category:

- `Export Asset To Godot`
- `Export Assets To Godot`
- `Export Content Folder To Godot`
- `Export All To Godot`

## Useful settings

| Option | Role |
|---|---|
| Godot Project Folder | Godot project folder (Tools → Godot Exporter) |
| Export Subfolder | Subfolder created at the destination (`assets/export` by default) |
| Source Content Path | Unreal root for Export All (`/Game` by default) |
| Use Binary Glb | `.glb` (recommended) or `.gltf` + sidecars |
| Include Dependencies | For a mesh / material / level, also export referenced textures and materials. An AnimSequence exports only the animation. |
| Skip Unchanged | Skip an asset if the Godot file is already newer |
| Auto Export On Save | Re-export a supported asset as soon as it is saved |
| Prefer Original Source Files | Copy the imported PNG/WAV/OGG if it still exists |
| Export Prefabs | Write a `.tscn` in `prefabs/` for each mesh |

## Dependency

The Unreal **glTF Exporter** plugin (`GLTFExporter`) must stay enabled: meshes, animations, and levels go through it.
