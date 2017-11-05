#include <iostream>
using namespace std;
#include "Interpreteur.h"
#include "Exceptions.h"

int main(int argc, char* argv[]) {
    string nomFich;
    if (argc < 3) {
        cout << "Usage : " << argv[0] << " nom_fichier_source" << endl << endl;
        cout << "Entrez le nom du fichier que voulez-vous interpréter : ";
        getline(cin, nomFich);
    } else if (argc == 3 && argv[1][0] == '-') {
        nomFich = argv[2];
        ifstream fichier(nomFich.c_str());
        if (argv[1][1] == 'c') {
            string nomFichRaccourcis = nomFich.substr(0, nomFich.size() - 4); // on enlève .txt
            try {
                Interpreteur interpreteur(fichier);
                interpreteur.analyse();
                // Si pas d'exception levée, l'analyse syntaxique a réussi
                cout << endl << "================ Syntaxe Correcte" << endl;
                // On affiche le contenu de la table des symboles avant de compiler le programme
                cout << endl << "================ Table des symboles avant compilation : " << interpreteur.getTable();
                cout << endl << "================ compilation" << endl;
                ofstream fichierCpp(nomFichRaccourcis+".cpp");
                interpreteur.traduitEnCPP(fichierCpp, 0);
                cout << "code en c++ enregistré dans : " << nomFichRaccourcis+".cpp" << endl;
                fichierCpp.close();
                system(("g++ " + nomFichRaccourcis + ".cpp -o " + nomFichRaccourcis).c_str());
            } catch (InterpreteurException & e) {
                cout << e.what() << endl;
            }
        } else if (argv[1][1] == 'i') {
            try {
                Interpreteur interpreteur(fichier);
                interpreteur.analyse();
                // Si pas d'exception levée, l'analyse syntaxique a réussi
                cout << endl << "================ Syntaxe Correcte" << endl;
                // On affiche le contenu de la table des symboles avant d'exécuter le programme
                cout << endl << "================ Table des symboles avant exécution : " << interpreteur.getTable();
                cout << endl << "================ Execution de l'arbre" << endl;
                // On exécute le programme si l'arbre n'est pas vide
                if (interpreteur.getArbre() != nullptr) interpreteur.getArbre()->executer();
                // Et on vérifie qu'il a fonctionné en regardant comment il a modifié la table des symboles
                cout << endl << "================ Table des symboles apres exécution : " << interpreteur.getTable();
            } catch (InterpreteurException & e) {
                cout << e.what() << endl;
            }
            return 0;
        } else {
            if (argc != 3) cout << "nombre d'argument incorecte" << endl;
            cout << "l'option : " << argv[1] << " n'existe pas, liste des options disponibles : " << endl;
            cout << "-i : interprétation" << endl;
            cout << "-c : compilation" << endl;
            cout << "-h : help" << endl;
        }

    } else if (argc == 2 && argv[1] == "-h") {
        cout << "options disponibles : " << endl;
        cout << "-i : interprétation" << endl;
        cout << "-c : compilation" << endl;
        cout << "-h : help" << endl;
        cout << "autre informations : " << endl;
        cout << "par défaut la compilation laisse un fichier trace : nomdevotrefichier.cpp" << endl;
        cout << "le compilateur utilisé par défaut est g++" << endl;
    } else {
        cout << "Erreur, arguments invalide, liste des options disponibles : " << endl;
        cout << "-i : interprétation" << endl;
        cout << "-c : compilation" << endl;
        cout << "-h : help" << endl;
    }
}