# ScanTailor Restoration V1 alpha — modifications de l’interface

## Accès

Un nouvel élément **Tools → Restoration Lab (V1 alpha)…** ouvre le panneau expérimental. Il ne remplace aucune étape historique de ScanTailor et ne modifie pas les projets existants.

## Onglet Géométrie

- **Méthode de redressement** : combinée, lignes de texte ou bords de page.
- **Confiance minimale** : seuil prévu pour refuser une correction automatique peu fiable.

Dans cette alpha, ces valeurs sont enregistrées comme profil. Le branchement au moteur de traitement est prévu pour l’alpha suivante.

## Onglet Nettoyage

- normalisation du fond papier ;
- taille maximale des poussières ;
- sensibilité ;
- protection de la ponctuation et des accents.

## Onglet Encre

- choix Otsu, gaussien adaptatif, Sauvola ou Wolf-Jolion ;
- rayon local ;
- contraste minimal ;
- protection des empattements et traits fins ;
- réparation morphologique légère, désactivée par défaut.

## Onglet Export

Sélection des sorties prévues :

- master de conservation en niveaux de gris ;
- image de lecture améliorée ;
- image noir et blanc pour OCR ;
- masque PNG des pixels supprimés ;
- trace JSON des paramètres.

## Persistance

Les réglages sont stockés avec `QSettings`, dans le groupe `restorationLabV1`. Le bouton **Restore Defaults** restaure le profil prudent recommandé.

## Limites de cette V1

Cette livraison est une **alpha d’interface**. Elle ajoute un panneau de configuration fonctionnel, persistant et compilable, mais **les nouveaux paramètres ne pilotent pas encore le pipeline d’image** : rien de ce qui est réglé ici n’a d’effet sur les pages traitées. Les fonctions existantes de ScanTailor restent inchangées.

## Implémentation

| Fichier | Rôle |
| --- | --- |
| `src/app/RestorationLabDialog.h` / `.cpp` | Le dialogue, construit entièrement en code (pas de fichier `.ui`). |
| `src/app/ui/MainWindow.ui` | Action `actionRestorationLab` et son entrée dans le menu Tools. |
| `src/app/MainWindow.cpp` / `.h` | Slot `openRestorationLabDialog()` et sa connexion. |
| `src/app/CMakeLists.txt` | Ajout des sources à `gui_only_sources`. |
