#include "Interpreteur.h"
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

Interpreteur::Interpreteur(ifstream & fichier) :
m_lecteur(fichier), m_table(), m_arbre(nullptr) {
}

void Interpreteur::analyse() {
    m_arbre = programme(); // on lance l'analyse de la première règle
}

//methode pour la traduction en c++

void Interpreteur::traduitEnCPP(ostream & cout, unsigned int indentation)const {
    cout << "#include <iostream>" << endl << endl;
    cout << setw(4 * indentation) << "" << "int main() {" << endl << endl;
    // Début d’un programme C++
    // Ecrire en C++ la déclaration des variables présentes dans le programme... 
    // ... variables dont on retrouvera le nom en parcourant la table des symboles ! 
    // Par exemple, si le programme contient i,j,k, il faudra écrire : int i; int j; int k; ... 
    cout << "//##################################################" << endl;
    cout << "//#############declaration des variables############" << endl;
    cout << "//##################################################" <<endl;
    for (int i = 0; i < getTable().getTaille(); i++) {
        Symbole symbole = getTable()[i];
        if (symbole.estUneVariable())  cout << "int " << symbole.getChaine() << "; ";
    }
    cout <<endl << "//##################################################" << endl << endl;
    cout << "//début du programme" <<endl <<endl;
    getArbre()->traduitEnCPP(cout, indentation + 1);
    // lance l'opération traduitEnCPP sur la racine
    cout << setw(4 * (indentation + 1)) << "" << "return 0;" << endl;
    cout << setw(4 * indentation) << "}" << endl; // Fin d’un programme C++
}

void Interpreteur::tester(const string & symboleAttendu) const throw (SyntaxeException) {
    // Teste si le symbole courant est égal au symboleAttendu... Si non, lève une exception
    static char messageWhat[256];
    if (m_lecteur.getSymbole() != symboleAttendu) {
        sprintf(messageWhat,
                "Ligne %d, Colonne %d - Erreur de syntaxe - Symbole attendu : %s - Symbole trouvé : %s",
                m_lecteur.getLigne(), m_lecteur.getColonne(),
                symboleAttendu.c_str(), m_lecteur.getSymbole().getChaine().c_str());
        throw SyntaxeException(messageWhat);
    }
}

void Interpreteur::testerEtAvancer(const string & symboleAttendu) throw (SyntaxeException) {
    // Teste si le symbole courant est égal au symboleAttendu... Si oui, avance, Sinon, lève une exception
    tester(symboleAttendu);
    m_lecteur.avancer();
}

void Interpreteur::erreur(const string & message) const throw (SyntaxeException) {
    // Lève une exception contenant le message et le symbole courant trouvé
    // Utilisé lorsqu'il y a plusieurs symboles attendus possibles...
    static char messageWhat[256];
    sprintf(messageWhat,
            "Ligne %d, Colonne %d - Erreur de syntaxe - %s - Symbole trouvé : %s",
            m_lecteur.getLigne(), m_lecteur.getColonne(), message.c_str(), m_lecteur.getSymbole().getChaine().c_str());
    throw SyntaxeException(messageWhat);
}

Noeud* Interpreteur::programme() {
    // <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
    testerEtAvancer("procedure");
    testerEtAvancer("principale");
    testerEtAvancer("(");
    testerEtAvancer(")");
    Noeud* sequence = seqInst();
    testerEtAvancer("finproc");
    tester("<FINDEFICHIER>");
    return sequence;
}

Noeud* Interpreteur::seqInst() {
    // <seqInst> ::= <inst> { <inst> }
    NoeudSeqInst* sequence = new NoeudSeqInst();
    do {
        sequence->ajoute(inst());
    } while (m_lecteur.getSymbole() == "<VARIABLE>" ||
            m_lecteur.getSymbole() == "si" ||
            m_lecteur.getSymbole() == "tantque" ||
            m_lecteur.getSymbole() == "repeter" ||
            m_lecteur.getSymbole() == "pour" ||
            m_lecteur.getSymbole() == "ecrire" ||
            m_lecteur.getSymbole() == "lire");
    // Tant que le symbole courant est un début possible d'instruction...
    // Il faut compléter cette condition chaque fois qu'on rajoute une nouvelle instruction
    return sequence;
}

Noeud* Interpreteur::inst() {
    // <inst> ::= <affectation>  ; | <instSi>
    try {

        if (m_lecteur.getSymbole() == "<VARIABLE>") {
            Noeud *affect = affectation();
            testerEtAvancer(";");
            return affect;
        } else if (m_lecteur.getSymbole() == "si") {
            return instSiRiche();
        } else if (m_lecteur.getSymbole() == "tantque") {
            return instTantQue();
        } else if (m_lecteur.getSymbole() == "repeter") {
            return instRepeter();
        } else if (m_lecteur.getSymbole() == "pour") {
            return instPour();
        } else if (m_lecteur.getSymbole() == "ecrire") {
            return instEcrire();
        } else if (m_lecteur.getSymbole() == "lire") {
            return instLire();
        }// Compléter les alternatives chaque fois qu'on rajoute une nouvelle instruction
        else {
            erreur("Instruction incorrecte");
        }
    } catch (SyntaxeException e) {
        cout << e.what() << endl;
        m_arbre = nullptr;
        m_lecteur.avancer();
        do {
            m_lecteur.avancer();
        } while (m_lecteur.getSymbole() != "<VARIABLE>" &&
                m_lecteur.getSymbole() != "si" &&
                m_lecteur.getSymbole() != "tantque" &&
                m_lecteur.getSymbole() != "repeter" &&
                m_lecteur.getSymbole() != "pour" &&
                m_lecteur.getSymbole() != "ecrire" &&
                m_lecteur.getSymbole() != "lire" &&
                m_lecteur.getSymbole() != "<FINDEFICHIER>");

    }

}

Noeud* Interpreteur::affectation() {
    // <affectation> ::= <variable> = <expression> 
    tester("<VARIABLE>");
    Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table eton la mémorise
    m_lecteur.avancer();
    testerEtAvancer("=");
    Noeud* exp = expression(); // On mémorise l'expression trouvée
    return new NoeudAffectation(var, exp); // On renvoie un noeud affectation
}

Noeud* Interpreteur::expression() {
    // <expression> ::= <terme> { + <terme> | - <terme> }
    Noeud* trm = terme();
    while (m_lecteur.getSymbole() == "+" || m_lecteur.getSymbole() == "-") {
        Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud* tDroit = terme(); // On mémorise l'opérande droit
        trm = new NoeudOperateurBinaire(operateur, trm, tDroit); // Et on construit un noeud opérateur binaire
    }
    return trm; // On renvoie trm qui pointe sur la racine de l'expression
}

Noeud* Interpreteur::terme() {
    // <terme> ::= <facteur> { * <facteur> | / <facteur> }
    Noeud* fact = facteur();
    while (m_lecteur.getSymbole() == "*" || m_lecteur.getSymbole() == "/") {
        Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud* factDroit = facteur(); // On mémorise l'opérande droit
        fact = new NoeudOperateurBinaire(operateur, fact, factDroit); // Et on construit un noeud opérateur binaire
    }
    return fact; // On renvoie fact qui pointe sur la racine du terme
}

Noeud* Interpreteur::facteur() {
    // <facteur> ::= <entier>  |  <variable>  |  - <expBool>  | non <expBool> | ( <expBool> )
    Noeud* fact = nullptr;
    if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>") {
        fact = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
        m_lecteur.avancer();
    } else if (m_lecteur.getSymbole() == "-") { // - <expBool>
        m_lecteur.avancer();
        // on représente le moins unaire (- expBool) par une soustraction binaire (0 - expBool)
        fact = new NoeudOperateurBinaire(Symbole("-"), m_table.chercheAjoute(Symbole("0")), expBool());
    } else if (m_lecteur.getSymbole() == "non") { // non <expBool>
        m_lecteur.avancer();
        fact = new NoeudOperateurBinaire(Symbole("non"), expBool(), nullptr);
    } else if (m_lecteur.getSymbole() == "(") { // expression parenthésée
        m_lecteur.avancer();
        fact = expBool();
        testerEtAvancer(")");
    } else
        erreur("Facteur incorrect");
    return fact;
}

Noeud* Interpreteur::expBool() {
    // <expBool> ::= <relationET> { ou <relationET> }
    Noeud* relEt = relationET();
    while (m_lecteur.getSymbole() == "ou") {
        m_lecteur.avancer();
        Noeud* relEtDroit = relationET(); // On mémorise l'opérande droit
        relEt = new NoeudOperateurBinaire(Symbole("ou"), relEt, relEtDroit); // Et on construit un noeud opérateur binaire
    }
    return relEt; // On renvoie relEt qui pointe sur la racine de l'expression Booleenne   
}

Noeud* Interpreteur::relationET() {
    // <relationET> ::= <relation> { et <relation> }
    Noeud* rel = relation();
    while (m_lecteur.getSymbole() == "et") {
        m_lecteur.avancer();
        Noeud* relDroit = relation(); // On mémorise l'opérande droit
        rel = new NoeudOperateurBinaire(Symbole("et"), rel, relDroit); // Et on construit un noeud opérateur binaire
    }
    return rel; // On renvoie rel qui pointe sur la racine de la relation "et"   
}

Noeud* Interpreteur::relation() {
    // <relation> ::= <expression> { <opRel> <expression> }
    // <opRel> ::= <= | >= | == | !=
    Noeud* expr = expression();
    while ( m_lecteur.getSymbole() == "<"  || m_lecteur.getSymbole() == ">"  ||
            m_lecteur.getSymbole() == "<=" || m_lecteur.getSymbole() == ">=" ||
            m_lecteur.getSymbole() == "==" || m_lecteur.getSymbole() == "!=") {
        Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud* exprDroit = expression(); // On mémorise l'opérande droit
        expr = new NoeudOperateurBinaire(operateur, expr, exprDroit); // Et on construit un noeud opérateur binaire
    }
    return expr; // On renvoie expr qui pointe sur la racine de la relation
}

Noeud* Interpreteur::instSi() {
    // <instSi> ::= si ( <expression> ) <seqInst> finsi
    testerEtAvancer("si");
    testerEtAvancer("(");
    Noeud* condition = expression(); // On mémorise la condition
    testerEtAvancer(")");
    Noeud* sequence = seqInst(); // On mémorise la séquence d'instruction
    testerEtAvancer("finsi");
    return new NoeudInstSi(condition, sequence); // Et on renvoie un noeud Instruction Si
}

Noeud* Interpreteur::instTantQue() {
    // <instTantQue> ::= si( <expression> ) <seqInst> { fintantque
    testerEtAvancer("tantque");
    testerEtAvancer("(");
    Noeud* condition = expression();
    testerEtAvancer(")");
    Noeud* sequence = seqInst();
    testerEtAvancer("fintantque");
    return new NoeudInstTantQue(condition, sequence);
}

Noeud* Interpreteur::instSiRiche() {
    // <instSiRiche> ::= si( <expression> ) <seqInst> { sinonsi ( <expression> ) <seqInst> } [ sinon <sequinst> ] finsi

    testerEtAvancer("si");
    testerEtAvancer("(");
    NoeudInstSiRiche* siRiche = new NoeudInstSiRiche();
    Noeud* condition = expression(); // on mémorise la condition
    testerEtAvancer(")");
    Noeud* sequence = seqInst(); // on mémorise la séquence d'instruction
    siRiche->ajoute(new NoeudInstSi(condition, sequence));
    while (m_lecteur.getSymbole() == "sinonsi") { // on mémorise les conditions et les instructions pour les sinons
        m_lecteur.avancer(); // on fait avancer le lecteur sur le symbole suivant
        testerEtAvancer("(");
        condition = expression();
        testerEtAvancer(")");
        sequence = seqInst();
        siRiche->ajoute(new NoeudInstSi(condition, sequence));
    }
    if (m_lecteur.getSymbole() == "sinon") { // si il y a un sinon on ajoute la condition et la séquence d'instruction
        m_lecteur.avancer();
        sequence = seqInst();
        siRiche -> ajoute(new NoeudInstSi(m_table.chercheAjoute(Symbole("1")), sequence)); //toujours vrai
    }
    testerEtAvancer("finsi"); //on vérifie que le si se termine bien;
    return siRiche;
}

Noeud* Interpreteur::instRepeter() {
    // <instRepeter> ::= repeter <seqInst> jusqua (<expression>)

    testerEtAvancer("repeter");
    Noeud* sequence = seqInst();
    testerEtAvancer("jusqua");
    testerEtAvancer("(");
    Noeud* condition = expression();
    testerEtAvancer(")");
    return new NoeudInstRepeter(condition, sequence);
}

Noeud* Interpreteur::instPour() {
    // <instPour> ::= pour ([ <affectation> ];<expression> ;[ <affectation> ])<seqInst> finpour
    testerEtAvancer("pour");
    testerEtAvancer("(");
    Noeud* affectation1 = nullptr;
    if (m_lecteur.getSymbole() == "<VARIABLE>") {
        affectation1 = affectation();
    }
    testerEtAvancer(";");
    Noeud* condition = expression();
    testerEtAvancer(";");
    Noeud* affectation2 = nullptr;
    if (m_lecteur.getSymbole() == "<VARIABLE>") {
        affectation2 = affectation();
    }
    testerEtAvancer(")");
    Noeud* sequence = seqInst();
    testerEtAvancer("finpour");
    return new NoeudInstPour(affectation1, affectation2, condition, sequence);
}

Noeud* Interpreteur::instEcrire() {
    // <instEcrire>::= ecrire (<expression> | <chaine> { , <excpresison> | <chaine> } )
    NoeudInstEcrire * ecrire = new NoeudInstEcrire();
    testerEtAvancer("ecrire");
    testerEtAvancer("(");
    Noeud *param;
    string chaine;
    if (m_lecteur.getSymbole() == "<CHAINE>") {
        param = m_table.chercheAjoute(m_lecteur.getSymbole());
        m_lecteur.avancer();
    } else {
        param = expression();
    }
    ecrire->ajoute(param);
    while (m_lecteur.getSymbole() == ",") {
        m_lecteur.avancer();
        if (m_lecteur.getSymbole() == "<CHAINE>") {
            param = m_table.chercheAjoute(m_lecteur.getSymbole());
            m_lecteur.avancer();
        } else {
            param = expression();
        }
        ecrire->ajoute(param);
    }
    testerEtAvancer(")");
    return ecrire;
}


//je ne sais pas si l'analyse syntaxique est correcte 
//je me suis inspiré du ecrire avec des variables
//le code compile et m'a l'air logique mais je suis pas sur
//j'ai ajouté lire a inst() et seqInst()

Noeud* Interpreteur::instLire() {
    // <instLire>::= lire ( <variable> {,<variable>} )
    testerEtAvancer("lire");
    testerEtAvancer("(");
    Noeud* noeudLire = new NoeudInstLire();
    tester("<VARIABLE>");
    Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table et on la mémorise
    //facteur pour l'affectation
    noeudLire->ajoute(var);
    m_lecteur.avancer();
    while (m_lecteur.getSymbole() == ",") {
        m_lecteur.avancer();
        tester("<VARIABLE>");
        var = m_table.chercheAjoute(m_lecteur.getSymbole());
        noeudLire->ajoute(var);
        m_lecteur.avancer();
    }
    testerEtAvancer(")");
    return noeudLire;
}

Noeud* Interpreteur::instSelon() {
    
    Noeud* instSelon = new NoeudInstSelon(); //instruction rendue
    testerEtAvancer("selon");
    testerEtAvancer("(");
    tester("<VARIABLE>");//peut etre mettre entier?
    Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table et on la mémorise
    m_lecteur.avancer();
    testerEtAvancer(")");
    
    do {
        testerEtAvancer("cas");
        tester("<ENTIER>");
        Noeud* numeroCas = m_table.chercheAjoute(m_lecteur.getSymbole());
        Noeud* node = new NoeudOperateurBinaire(Symbole("=="),var,numeroCas);
        m_lecteur.avancer();
        testerEtAvancer(":");
        instSelon->ajoute(seqInst());
    }
    while ( m_lecteur.getSymbole() == "cas");
    
    if (m_lecteur.getSymbole() == "defaut") {
        testerEtAvancer("defaut");
        testerEtAvancer(":");
        instSelon->ajoute(seqInst());
    }
    testerEtAvancer("finSelon");
    return instSelon;
}
