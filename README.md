# Suivi et comptage de véhicules par traitement d'image

Programme C++ qui détecte et compte les véhicules passant sur une route à partir d'une vidéo de vidéosurveillance, avec **OpenCV**. Le comptage se fait séparément pour deux voies de circulation, grâce à deux zones de détection.

Le détail de la démarche et des résultats se trouve dans `Rapport Traitement Image.pdf`.

## Principe

Pour chaque image de la vidéo :

1. **Détection des voies** : à la 10ᵉ image, on applique un filtre de Canny puis une transformée de Hough pour repérer les lignes (bordures des voies), tracées ensuite en rouge sur la vidéo.
2. **Soustraction de fond** : un modèle de fond (`BackgroundSubtractorKNN`) isole les objets en mouvement.
3. **Nettoyage du masque** : filtre médian (bruit « poivre et sel »), binarisation par seuillage, puis érosion et dilatation morphologiques.
4. **Détection des véhicules** : extraction des contours et tracé d'un rectangle englobant (en bleu) autour de chaque objet détecté.
5. **Comptage** : deux zones de détection (rectangles verts) correspondent chacune à une voie. Un véhicule est compté lorsque son rectangle franchit la zone correspondante. Les deux compteurs s'affichent à l'écran.

## Structure du dépôt

| Fichier | Description |
|---|---|
| `main.cpp` | Point d'entrée : ouvre la vidéo, la lit, puis la ferme |
| `camera.h` / `camera.cpp` | Classe `Camera` : ouverture de la source (fichier ou caméra), traitement et affichage |
| `cctv.avi` | Vidéo de test |
| `Makefile` | Compilation (g++ et OpenCV 4 via `pkg-config`) |
| `Rapport Traitement Image.pdf` | Rapport du projet |
| `.vscode/` | Configuration de l'éditeur |

## Prérequis

- `g++`
- [OpenCV 4](https://opencv.org) (accessible via `pkg-config`)
- `pkg-config`

Exemple d'installation sous Ubuntu / Debian :

```bash
sudo apt install g++ pkg-config libopencv-dev
```

## Compilation

```bash
make
```

L'exécutable généré s'appelle `Tp`. Pour supprimer les fichiers compilés :

```bash
make clean
```

## Utilisation

```bash
./Tp
```

Le programme lit `cctv.avi` et affiche la vidéo annotée dans une fenêtre :

- lignes rouges : bordures des voies détectées ;
- rectangles bleus : objets en mouvement détectés ;
- rectangles verts : zones de comptage ;
- compteurs : nombre de véhicules comptés pour chaque voie.

Appuyer sur **Échap** pour quitter. Pour utiliser une autre source, modifier l'argument passé à `myCam.open(...)` dans `main.cpp`. Il accepte un chemin de fichier vidéo ou un numéro de caméra (par exemple `"0"`).

## Limites

Les zones de comptage et les seuils (taille des éléments structurants, seuil de binarisation, paramètres de Hough) sont réglés **pour la vidéo `cctv.avi`** et définis directement dans le code. Pour une autre vidéo ou un autre angle de caméra, ces valeurs doivent être ajustées.

## Auteur

[PierreAlexandre13](https://github.com/PierreAlexandre13)
