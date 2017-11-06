#ifndef ARBREABSTRAIT_H
#define ARBREABSTRAIT_H

// Contient toutes les déclarations de classes nécessaires
//  pour représenter l'arbre abstrait


#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;

#include "Symbole.h"
#include "Exceptions.h"

////////////////////////////////////////////////////////////////////////////////

class Noeud {
    // Classe abstraite dont dériveront toutes les classes servant à représenter l'arbre abstrait
    // Remarque : la classe ne contient aucun constructeur
public:
virtual int executer() = 0; // Méthode pure (non implémentée) qui rend la classe abstraite

    virtual void ajoute(Noeud* instruction) { throw OperationInterditeException(); }
    virtual void traduitEnCPP(ostream & cout,unsigned int indentation) const = 0;

    virtual ~Noeud() {} // Présence d'un destructeur virtuel conseillée dans les classes abstraites
};

////////////////////////////////////////////////////////////////////////////////

class NoeudSeqInst : public Noeud {
    // Classe pour représenter un noeud "sequence d'instruction"
    //  qui a autant de fils que d'instructions dans la séquence
public:
    NoeudSeqInst(); // Construit une séquence d'instruction vide

    ~NoeudSeqInst() {
    } // A cause du destructeur virtuel de la classe Noeud
    int executer(); // Exécute chaque instruction de la séquence
    void ajoute(Noeud* instruction); // Ajoute une instruction à la séquence
    void traduitEnCPP(ostream & cout,unsigned int indentation) const override;

private:
    vector<Noeud *> m_instructions; // pour stocker les instructions de la séquence
};

////////////////////////////////////////////////////////////////////////////////

class NoeudAffectation : public Noeud {
    // Classe pour représenter un noeud "affectation"
    //  composé de 2 fils : la variable et l'expression qu'on lui affecte
public:
    NoeudAffectation(Noeud* variable, Noeud* expression); // construit une affectation
    void traduitEnCPP(ostream & cout,unsigned int indentation) const override;

    ~NoeudAffectation() {
    } // A cause du destructeur virtuel de la classe Noeud
    int executer(); // Exécute (évalue) l'expression et affecte sa valeur à la variable

private:
    Noeud* m_variable;
    Noeud* m_expression;
};

////////////////////////////////////////////////////////////////////////////////

class NoeudOperateurBinaire : public Noeud {
    // Classe pour représenter un noeud "opération binaire" composé d'un opérateur
    //  et de 2 fils : l'opérande gauche et l'opérande droit
public:
    NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit);
    // Construit une opération binaire : operandeGauche operateur OperandeDroit
    void traduitEnCPP(ostream & cout,unsigned int indentation) const override;

    ~NoeudOperateurBinaire() {
    } // A cause du destructeur virtuel de la classe Noeud
    int executer(); // Exécute (évalue) l'opération binaire)

private:
    Symbole m_operateur;
    Noeud* m_operandeGauche;
    Noeud* m_operandeDroit;
};

////////////////////////////////////////////////////////////////////////////////

class NoeudInstSi : public Noeud {
    // Classe pour représenter un noeud "instruction si"
    //  et ses 2 fils : la condition du si et la séquence d'instruction associée
public:
    NoeudInstSi(Noeud* condition, Noeud* sequence);
    // Construit une "instruction si" avec sa condition et sa séquence d'instruction
    
    void traduitEnCPP(ostream & cout,unsigned int indentation) const override;
    ~NoeudInstSi() {
    } // A cause du destructeur virtuel de la classe Noeud
    int executer(); // Exécute l'instruction si : si condition vraie on exécute la séquence
    
private:
    Noeud* m_condition;
    Noeud* m_sequence;
};

////////////////////////////////////////////////////////////////////////////////

class NoeudInstTantQue : public Noeud {
    // Classe pour représenter un noeud "instruction tant que"
    //et ses 2 fils : la condition du tantque et la séquence d'instruction associée
public:
    NoeudInstTantQue(Noeud* condition, Noeud* sequence);
    //construit une "instruction tantque" avec sa condition et sa sequence d'instruction

    ~NoeudInstTantQue() {
    } // A cause de la classe Noeud
    int executer() override;
    void traduitEnCPP(ostream& cout, unsigned int indentation) const override;
    

private:
    Noeud* m_condition;
    Noeud* m_sequence;
};


////////////////////////////////////////////////////////////////////////////////

class NoeudInstSiRiche : public Noeud {
    // Classe pour représenter un noeud "instruction si riche"
    //et ses n fils : les conditions du tantque et les séquence d'instructions associées
public:
    NoeudInstSiRiche();
    void ajoute(Noeud* instSi) override;

    ~NoeudInstSiRiche() {
    }
    int executer() override;
    
    void traduitEnCPP(ostream& cout, unsigned int indentation) const override;
private:
    vector<Noeud*> m_instSis;

};

////////////////////////////////////////////////////////////////////////////////

class NoeudInstRepeter : public Noeud {
    // Classe pour représenter un noeud "instruction repeter"
    // elle contient la sequence d'unstruction et la condition

public:
    NoeudInstRepeter(Noeud* condition, Noeud* sequence);

    ~NoeudInstRepeter() {
    }
    int executer() override;
    void traduitEnCPP(ostream& cout, unsigned int indentation) const override;

private:
    Noeud* m_condition;
    Noeud* m_sequence;
};
////////////////////////////////////////////////////////////////////////////////

class NoeudInstPour : public Noeud {
    // Classe pour représenter un noeud "instruction pour"
    // elle contient ...

public:
    NoeudInstPour(Noeud* affectation1, Noeud* affectation2, Noeud* condition, Noeud* sequence);
    int executer() override;
    void traduitEnCPP(ostream& cout, unsigned int indentation) const override;

private:
    Noeud* m_affectation1;
    Noeud* m_affectation2;
    Noeud* m_condition;
    Noeud* m_sequence;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstEcrire : public Noeud {
    // Classe pour représenter un noeud "instrution écrire"
    // elle contient un vecteur de parametres

public:
    NoeudInstEcrire();
    void ajoute(Noeud* param) override;
    int executer() override;
    void traduitEnCPP(ostream& cout, unsigned int indentation) const override;
    
    virtual ~NoeudInstEcrire() {}
    
private:
    vector<Noeud*> m_params;

};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstLire : public Noeud {
    // Classe pour représenter un noeud "instrution écrire"
    // elle contient ...
public:
    NoeudInstLire();
    int executer() override;
    void ajoute(Noeud* variable) override;
    void traduitEnCPP(ostream& cout, unsigned int indentation) const override;
    
    virtual ~NoeudInstLire() {}
    
private:
    vector<Noeud*> m_variables;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstSelon : public Noeud {
    // Classe pour représenter un noeud "instrution Selon"
    
public:
    //NoeudInstSelon(Noeud* variable, Noeud* casNum, Noeud* sequence,std::vector<Noeud*> cas, Noeud* defaut);
    int executer();
    //void traduitEnCPP... si tu veux le faire
    
private :
    /*Noeud* m_variable;
    Noeud* m_casNum;
    Noeud* m_sequence;
    std::vector<Noeud*> m_cas;
    Noeud* m_defaut;*/
};


#endif /* ARBREABSTRAIT_H */

