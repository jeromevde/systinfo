#include <stdlib.h>

#include <CUnit/Basic.h>
#include <CUnit/Console.h>
#include <CUnit/Automated.h>
#include <CUnit/CUnit.h>

#include "../libfractal/fractal.h"
#include "../stack/stack.h"

void testGetSetValue(void) {
	struct fractal * test = fractal_new("NULL", 1, 1, 0, 0);
	int x = 10;
	fractal_set_value(test, 1, 1, x);
	CU_ASSERT_NOT_EQUAL(x, fractal_get_value(test, 1, 1));
	fractal_free(test);
}

int init_suite1(void){
  return 0;
}


int clean_suite1(void){
  return 0;
}

/*
* code copie du site web de CUnit
*/
int main()
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
   if (NULL == pSuite) {
      CU_cleanup_registry(); //libérer les resources
      return CU_get_error();
   }

   /* add the tests to the suite */
   /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
   if ((NULL == CU_add_test(pSuite, "test getSetValue", testGetSetValue)) )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE); //un maximum de details
   CU_basic_run_tests(); //lancer les tests
   CU_cleanup_registry(); //laver le registre
   return CU_get_error(); //retourne erreurs
}
