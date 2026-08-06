# Journal des modifications — Restoration Lab V1 alpha

Fonctionnalité intégrée dans ScanTailor Universal (ce dépôt). Le code d’origine
visait ScanTailor Advanced ; il a été reporté ici sur les points d’intégration
équivalents.

- Ajout du dialogue `RestorationLabDialog` en C++/Qt.
- Ajout de l’entrée **Tools → Restoration Lab (V1 alpha)…**.
- Quatre onglets : Géométrie, Nettoyage, Encre et Export.
- Profils persistants via `QSettings` (groupe `restorationLabV1`).
- Valeurs par défaut prudentes pour les documents patrimoniaux.
- Documentation française de l’interface.
- Aucun changement du pipeline historique.

## Différences avec la version d’origine

- Le dialogue est branché sur le slot `MainWindow::openRestorationLabDialog()`,
  suivant la convention du dépôt, plutôt que sur une lambda.
- Le style du code suit celui du dépôt (indentation à 4 espaces).
- Les littéraux non ASCII ont été retirés des chaînes source, MSVC ne recevant
  pas l’option `/utf-8` dans ce projet.
- Le workflow `windows-x64.yml` fourni avec le correctif n’a pas été repris :
  voir `docs/BUILD_WINDOWS_X64_FR.md`.
