#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"
#include <iostream>
#include <typeinfo>

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
    for (unsigned int i = 0; i < m_instructions.size(); i++)
        m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
    if (instruction != nullptr) m_instructions.push_back(instruction);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
    int valeur = m_expression->executer(); // On exécute (évalue) l'expression
    ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
    return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
    int og, od, valeur;
    if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
    if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
    // Et on combine les deux opérandes en fonctions de l'opérateur
    if (this->m_operateur == "+") valeur = (og + od);
    else if (this->m_operateur == "-") valeur = (og - od);
    else if (this->m_operateur == "*") valeur = (og * od);
    else if (this->m_operateur == "==") valeur = (og == od);
    else if (this->m_operateur == "!=") valeur = (og != od);
    else if (this->m_operateur == "<") valeur = (og < od);
    else if (this->m_operateur == ">") valeur = (og > od);
    else if (this->m_operateur == "<=") valeur = (og <= od);
    else if (this->m_operateur == ">=") valeur = (og >= od);
    else if (this->m_operateur == "et") valeur = (og && od);
    else if (this->m_operateur == "ou") valeur = (og || od);
    else if (this->m_operateur == "non") valeur = (!og);
    else if (this->m_operateur == "/") {
        if (od == 0) throw DivParZeroException();
        valeur = og / od;
    }
    return valeur; // On retourne la valeur calculée
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstSi::executer() {
    if (m_condition->executer()) {
        m_sequence->executer();
        return 1;                   // on retourne 1 si le si est exécuté
    }
    return 0;                       // on retourne 0 sinon
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstTantQue
////////////////////////////////////////////////////////////////////////////////

NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstTantQue::executer() {
    while (m_condition->executer()) {
        m_sequence->executer();
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSiRiche
////////////////////////////////////////////////////////////////////////////////

NoeudInstSiRiche::NoeudInstSiRiche() 
: m_instSis() {
}

void NoeudInstSiRiche::ajoute(Noeud* instSi) {
    m_instSis.push_back(instSi);
}

int NoeudInstSiRiche::executer() {
    int exec=0;
    for (int i=0; i < m_instSis.size() && !exec; i++) { // on parcours le tableau et on vérifie que aucun si n'a été exécuté (exec = 0)
        exec = m_instSis[i] -> executer();
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstRepeter
////////////////////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter(Noeud* condition, Noeud* sequence) 
: m_condition(condition),m_sequence(sequence) {

}
int NoeudInstRepeter::executer(){
    do {
        m_sequence->executer();
    }
    while(m_condition->executer());
    
    return 0;
}


////////////////////////////////////////////////////////////////////////////////
// NoeudInstPour
////////////////////////////////////////////////////////////////////////////////

NoeudInstPour::NoeudInstPour(Noeud* affectation1, Noeud* affectation2, Noeud* condition, Noeud* sequence)
: m_affectation1(affectation1), m_affectation2(affectation2), m_condition(condition),m_sequence(sequence) {
    
}

int NoeudInstPour::executer(){
    for(m_affectation1->executer(); m_condition->executer(); m_affectation2->executer()){
        m_sequence->executer();
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstEcrire
////////////////////////////////////////////////////////////////////////////////

NoeudInstEcrire::NoeudInstEcrire() {
}

void NoeudInstEcrire::ajoute(Noeud* param) {
    m_params.push_back(param);
}

int NoeudInstEcrire::executer() {
    for (auto param : m_params) {
        if ( (typeid(*param)==typeid(SymboleValue)) && *((SymboleValue*)param)=="<CHAINE>" ) {
            string chaine = ((SymboleValue*)param)->getChaine();
            cout << chaine.substr(1,chaine.size()-2);
        } else {
           cout << param->executer();
        }
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstLire
////////////////////////////////////////////////////////////////////////////////


/*
 * c'est ici que ca merde
 * le probleme c'est que executer utlise cin sur un noeud ce qui n'est pas possible
 * j'ai essaye d'ajouter un operator >> aux classes symbole et symbole value (pour saisir)
 * mais ca n'a pas marché
 * j'ai pense a faire en sorte que l'on controle si ce qu'on met dans lire est une varibale (normal)
 * et apres on pouurait faire d'autres if pour chaque type de variable (char,string etc...)
 * mais ca m'a l'air moche
 */
NoeudInstLire::NoeudInstLire() {

}

void NoeudInstLire::ajoute(Noeud* affectation) {
    m_affectations.push_back(affectation);
}

int NoeudInstLire::executer() {
    for (auto affectation : m_affectations) {
        affectation->executer();
    }
    return 0;
}


