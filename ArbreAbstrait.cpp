#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"
#include <iostream>
#include <typeinfo>
#include <memory>

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


void NoeudSeqInst::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    for (auto instruction : m_instructions) {
        instruction->traduitEnCPP(cout, indentation);
        cout << endl;
    }
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

void NoeudAffectation::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    int valeur = m_expression->executer(); // On exécute (évalue) l'expression
    ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
    cout << setw(4*indentation)<<"";
    cout << (((SymboleValue*)m_variable)->getChaine()) << " = ";
    m_expression->traduitEnCPP(cout, 0); //on traduit l'expression en cpp avec une indentation de 0 car sur la même ligne
    if (indentation != 0) cout << ";";
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


void NoeudOperateurBinaire::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    string operateur;
    if(this->m_operateur == "et") {
        operateur = "&&";
    } else if (this->m_operateur == "ou") {
        operateur = "||";
    }else if (this->m_operateur == "non") {
        operateur = "!";
    } else {
        operateur = this->m_operateur.getChaine();
    }
    cout << setw(4*indentation)<<"";
    m_operandeGauche->traduitEnCPP(cout, 0); // On traduit l'opérande gauche
    cout << " " << operateur << " "; 
    m_operandeDroit->traduitEnCPP(cout, 0); // On traduit l'opérande droit
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

void NoeudInstSi::traduitEnCPP(ostream & cout,unsigned int indentation) const {
    cout << setw(4*indentation)<<""<<"if (";
    // Ecrit "if (" avec un décalage de 4*indentation espaces 
    m_condition->traduitEnCPP( cout, 0);
    // Traduit la condition en C++ sans décalage 
    cout << ") {"<< endl;
    // Ecrit ") {" et passe à la ligne 
    m_sequence->traduitEnCPP(cout, indentation+1);
    // Traduit en C++ la séquence avec indentation augmentée 
    cout << setw(4*indentation)<< "" << "}" << endl;
    // Ecrit "}" avec l'indentation initiale et passe à la ligne 
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

void NoeudInstTantQue::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    cout << setw(4*indentation)<<""<<"while (";
    // Ecrit "if (" avec un décalage de 4*indentation espaces 
    m_condition->traduitEnCPP( cout, 0);
    // Traduit la condition en C++ sans décalage 
    cout << ") {"<< endl;
    // Ecrit ") {" et passe à la ligne 
    m_sequence->traduitEnCPP(cout, indentation+1);
    // Traduit en C++ la séquence avec indentation augmentée 
    cout << setw(4*indentation)<< "" << "}" << endl;
    // Ecrit "}" avec l'indentation initiale et passe à la ligne 
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

void NoeudInstSiRiche::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    m_instSis[0]->traduitEnCPP(cout, indentation);
    for (int i = 1; i < m_instSis.size(); i++) {
        cout << setw(4*indentation)<<"";
        cout << "else ";
        m_instSis[1]->traduitEnCPP(cout,0); // on ajoute if sans indentation
    }
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


void NoeudInstRepeter::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    cout << setw(4*indentation)<<"" << "do {" << endl;
    m_sequence->traduitEnCPP(cout, indentation+1);
    cout << setw(4*indentation)<<"" << "} while (";
    m_condition->traduitEnCPP(cout, 0); //on met la condition avec une indentation de 0 après le while (
    cout << ");" << endl;
}

//////////////////////////////////////////((SymboleValue*) m_variable)->setValeur(valeur);//////////////////////////////////////
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

void NoeudInstPour::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    cout << setw(4*indentation)<<"" << "for (";
    m_affectation1->traduitEnCPP(cout, 0);
    cout << "; ";
    m_condition->traduitEnCPP(cout, 0);
    cout << "; ";
    m_affectation2->traduitEnCPP(cout,0);
    cout << ") {" << endl;
    m_sequence->traduitEnCPP(cout, indentation+1);
    cout << "}" << endl;
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

void NoeudInstEcrire::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    cout << setw(4*indentation)<<"" << "cout << ";
    for(int i; i < m_params.size()-1; i++) {
        m_params[i]->traduitEnCPP(cout, 0);
        cout << " << ";
    }
    m_params[m_params.size()-1]->traduitEnCPP(cout, 0);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstLire
////////////////////////////////////////////////////////////////////////////////


NoeudInstLire::NoeudInstLire() {

}

void NoeudInstLire::ajoute(Noeud* variable) {
    m_variables.push_back(variable);
}

int NoeudInstLire::executer() {
    for (auto var : m_variables) {
        int val;
        cin >> val;
        ((SymboleValue*) var)->setValeur(val);
    }
    return 0;
}

void NoeudInstLire::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSelon
////////////////////////////////////////////////////////////////////////////////

NoeudInstSelon::NoeudInstSelon() {

}

void NoeudInstSelon::ajoute(Noeud* instSi) {
    m_instSis.push_back(instSi);
}

int NoeudInstSelon::executer() {
    int exec=0;
    for (int i=0; i < m_instSis.size() && !exec; i++) { // on parcours le tableau et on vérifie que aucun si n'a été exécuté (exec = 0)
        exec = m_instSis[i] -> executer();
    }
    return 0;
}

void NoeudInstSelon::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    m_instSis[0]->traduitEnCPP(cout, indentation);
    for (int i = 1; i < m_instSis.size(); i++) {
        cout << setw(4*indentation)<<"";
        cout << "else ";
        m_instSis[1]->traduitEnCPP(cout,0); // on ajoute if sans indentation
    }
}
