/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   newtestclass.h
 * Author: khelifza
 *
 * Created on Nov 4, 2017, 10:03:26 PM
 */

#ifndef NEWTESTCLASS_H
#define NEWTESTCLASS_H

#include "../Interpreteur.h"
#include "../SymboleValue.h"
#include "../TableSymboles.h"
#include "../Exceptions.h"
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/Portability.h>
#include <cppunit/TestCaller.h>

class newtestclass : public CPPUNIT_NS::TestFixture {
    
    CPPUNIT_TEST_SUITE(TestSi);
    CPPUNIT_TEST(TestSi);
    CPPUNIT_TEST(testMethod);
    CPPUNIT_TEST(testFailedMethod);
    

    CPPUNIT_TEST_SUITE_END();

public:
    
    TestSi();
    virtual ~newtestclass();
    void setUp();
    void tearDown();

private:
    void testMethod();
    void testFailedMethod();
};

#endif /* NEWTESTCLASS_H */

