#include "widget.h"
#include <QApplication>

/// @todo
/// - Inclusion: pouvoir exporter les fichiers du subset manquants dans un nouveau dossier, en conservant les relative paths
///
/// - Doublons: Right-Click - Show In Explorer
///
/// - Doublons: afficher les résultats dans un Tree (clé FileID, et en-dessous les FilePaths)
///
/// - grouper par taille de fichier avant de calculer les Hashs
/// - Doublons: afficher les images des fichiers en plusieurs exemplaires
/// - Doublons: algorithme pour désigner les fichiers superflus
/// - Doublons: possibilité de copier les exemplaires superflus dans un dossier à part
/// - Doublons: vérifier que tous les exemplaires superflus dans le dossier sont aussi présents dans le dossier de référence
///
/// @done
/// - Inclusion: afficher des progress bars!
/// - ajouter la taille du fichier à la comparaison MD5
/// - faire du multi-threading pour le calcul des MD5
/// - mettre l'analyse des fichiers (calc MD5) dans un thread et afficher une barre de progrès
/// - vérifier que tous les MD5 présents dans un dossier sont aussi présents dans un autre dossier
/// - recherche de doublons dans un dossier

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

    return a.exec();
}
