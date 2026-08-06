# Compilation Windows 64 bits — ScanTailor Universal (avec Restoration Lab V1 alpha)

Ce document remplace le `BUILD_WINDOWS_X64_FR.md` livré avec le correctif
Restoration Lab : celui-ci décrivait l’arborescence de ScanTailor Advanced, dont
les dépendances et le nom de cible diffèrent de ce dépôt.

## Méthode recommandée : GitHub Actions

Le dépôt possède déjà un workflow Windows x64 complet, `windows 2022 Qt6`
(`.github/workflows/windows-Qt6.yml`). Il installe Qt 6.9.0 MSVC 2022 x64,
construit les dépendances via vcpkg avec le triplet maison
`x64-windows-dynamic-boost-static`, compile, lance les tests, puis empaquette
une version portable.

1. Onglet **Actions** → workflow **windows 2022 Qt6**.
2. **Run workflow** sur la branche voulue (ou simplement pousser sur `master`).
3. Télécharger l’artefact **`scantailor-universal-windows-x64-Qt6`**.

L’artefact contient `scantailor-universal.exe`, l’exécutable CLI, les DLL Qt
déployées par `windeployqt`, les DLL vcpkg, ainsi que les dossiers
`translations/`, `stylesheets/` et `docs/` que l’application cherche à côté de
l’exécutable.

Un workflow Qt5 équivalent existe également (`windows-Qt5.yml`) si une version
Qt 5 est nécessaire.

## Compilation locale (Visual Studio 2022 + vcpkg)

Prérequis : Visual Studio 2022 (charge de travail C++), CMake, vcpkg, et Qt 6
MSVC 2022 x64.

```powershell
# Aligner vcpkg sur le commit épinglé par la CI (voir « Épinglage de vcpkg »)
git -C C:\vcpkg fetch --depth 1 origin f1d4bbc72f183441403ba5107cb19d75a5abc2a2
git -C C:\vcpkg checkout --force FETCH_HEAD
C:\vcpkg\bootstrap-vcpkg.bat -disableMetrics

# Dépendances, avec les triplets fournis dans .build\st-vcpkg-triplets
$env:VCPKG_BUILD_TYPE = "Release"
C:\vcpkg\vcpkg install zlib tiff libpng openjpeg libjpeg-turbo exiv2 opengl `
    pkgconf libmupdf mujs harfbuzz[freetype] gumbo freetype jbig2dec `
    --overlay-triplets "$PWD\.build\st-vcpkg-triplets" `
    --triplet x64-windows-dynamic-boost-static

C:\vcpkg\vcpkg install boost-core boost-foreach boost-bind boost-multi-index `
    boost-mpl boost-test boost-iterator boost-intrusive boost-random `
    boost-range boost-type-traits `
    --overlay-triplets "$PWD\.build\st-vcpkg-triplets" `
    --triplet x64-windows-dynamic-boost-static

# Configuration + compilation
$env:CMAKE_PREFIX_PATH   = "C:\Qt\6.9.0\msvc2022_64"
$env:CMAKE_TOOLCHAIN_FILE = "C:\vcpkg\scripts\buildsystems\vcpkg.cmake"
$env:VCPKG_OVERLAY_TRIPLETS = "$PWD\.build\st-vcpkg-triplets"
$env:VCPKG_TARGET_TRIPLET = "x64-windows-dynamic-boost-static"

.build\build.ps1 `
    -DPath "C:\vcpkg\installed\x64-windows-dynamic-boost-static" `
    -ReleaseType "Release" `
    -DCMAKE_CXX_STANDARD=11 -DCMAKE_CXX_STANDARD_REQUIRED=ON `
    -DCMAKE_CXX_EXTENSIONS=OFF -DQtPkgList="Qt6" `
    -DVCPKG_TARGET_TRIPLET="x64-windows-dynamic-boost-static"
```

`build.ps1` configure et compile dans `build\`. L’exécutable est produit dans
`build\src\app\Release\scantailor-universal.exe`. Exécuter ensuite
`windeployqt` dessus pour copier les DLL Qt, et recopier `src\stylesheets` et
les fichiers `.qm` à côté de l’exécutable (c’est ce que fait l’étape
« Package portable x64 build » du workflow).

## Épinglage de vcpkg

Les deux workflows Windows épinglent l’arbre de ports de `C:\vcpkg` sur le
commit `f1d4bbc72f183441403ba5107cb19d75a5abc2a2` (microsoft/vcpkg,
2026-08-04) avant le bootstrap.

Raison : Codeberg a ré-empaqueté l’archive `gumbo-parser 0.13.2` sans changer
le numéro de version, ce qui invalide le SHA512 figé dans le port. Toute image
de runner dont le vcpkg est antérieur au 2026-08-04 échoue avec :

```
error: download from https://codeberg.org/gumbo-parser/gumbo-parser/archive/0.13.2.tar.gz had an unexpected hash
error: building gumbo:x64-windows failed with: BUILD_FAILED
```

`gumbo` est tiré par `libmupdf` (lecture des PDF en entrée). Le commit épinglé
contient le correctif amont (`[gumbo] update Codeberg archive SHA512
(port-version 1)`, port-version 1).

Si Codeberg ré-empaquète à nouveau l’archive, le symptôme reviendra : relever
alors le commit `VCPKG_COMMIT` dans les deux workflows vers un commit vcpkg
plus récent contenant le hash rafraîchi.

En dernier recours, `-DENABLE_MUPDF=off` supprime la dépendance à
`libmupdf`/`gumbo`, au prix de la lecture des PDF comme source d’images.

## Note sur le standard C++

Le projet est configuré pour C++11 (`cmake/default_cxxflags.cmake`), ce que la
CI Windows confirme avec `-DCMAKE_CXX_STANDARD=11`. Sur Linux avec un Qt 6
récent (6.10+), les en-têtes Qt exigent C++17 et la compilation échoue dans
`src/exporting/` : ajouter alors `-DCMAKE_CXX_STANDARD=17`. Ce point est
antérieur et sans rapport avec Restoration Lab.

## Le workflow du correctif n’a pas été repris

Le correctif Restoration Lab fournissait son propre `.github/workflows/windows-x64.yml`.
Il n’a pas été intégré, car il visait ScanTailor Advanced et ne fonctionnerait
pas ici :

- il cherche `scantailor-advanced.exe`, alors que la cible s’appelle `scantailor-universal` ;
- il ne récupère pas les sous-modules git (`submodules: recursive`) ;
- il installe un jeu de dépendances incomplet (ni exiv2, ni openjpeg, ni mupdf, ni harfbuzz) ;
- il n’utilise pas les triplets vcpkg maison de `.build\st-vcpkg-triplets`, indispensables ici ;
- il n’empaquette ni `stylesheets/` ni `translations/`.

Le workflow existant du dépôt fait déjà tout cela correctement ; seules
l’exécution manuelle (`workflow_dispatch`) et l’étape d’empaquetage ont été
ajoutées.
