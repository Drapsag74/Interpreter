/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   newtestclass.cpp
 * Author: khelifza
 *
 * Created on Nov 4, 2017, 10:03:27 PM
 */

#include "TestSi.h"



CPPUNIT_TEST_SUITE_REGISTRATION(newtestclass);

newtestclass::TestSi() {
}

newtestclass::~newtestclass() {
}

void newtestclass::setUp() {
}

void newtestclass::tearDown() {
}

void newtestclass::testMethod() {
    
    //on cherche le fichier de testSi
    //ensuite on cree un objet interpreteur
    ifstream inFile("test_Si_et_SiRiche.txt");
    Interpreteur interpreteur(inFile);
    
    
    //tests de l'interpreteur analyse/execution
    CPPUNIT_ASSERT_NO_THROW_MESSAGE("",interpreteur.analyse());
    CPPUNIT_ASSERT_NO_THROW_MESSAGE("",interpreteur.getArbre()->executer());
    
    //test Si
    const int J = 5;//permettra de comparer avec la variable j de TestSi.txt
    SymboleValue *symbolevalue = nullptr;
    TableSymboles table = interpreteur.getTable();
    
    //on cherche dans testSi.txt la variable j puis on la stocke dans symbolvalue
    //on effectue ensuite le test qui compare la Vraible constante a celle du fichier
    //on compare j et J
    symbolevalue = table.chercheAjoute(Symbole("j"));
    CPPUNIT_ASSERT_EQUAL(J,symbolevalue->executer());
}

void newtestclass::testFailedMethod() {
    CPPUNIT_ASSERT(false);
}

