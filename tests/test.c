#include <stdlib.h>
#include <CUnit/Basic.h>
#include <CUnit/Console.h>
#include <CUnit/Automated.h>
#include <CUnit/CUnit.h>

#include "../libfractal/fractal.h"
#include "../stack/stack.h"

/*
* SUITE de test CUnit
*/


/*
* tester les get et set de libfractale
*/
void testGetterSetter(void){
	fractal_t *myfractal = fractal_new("myfractal", 1000,1000, 0.5,0.5);
	fractal_set_value(myfractal, 0, 3, 666);


	CU_ASSERT_STRING_EQUAL(fractal_get_name(myfractal), "myfractal");
	CU_ASSERT_EQUAL((int)fractal_get_a(myfractal), (int)0.5 );
	CU_ASSERT_EQUAL((int)fractal_get_b(myfractal), (int)0.5 );
	CU_ASSERT_EQUAL(fractal_get_width(myfractal), 1000);
	CU_ASSERT_EQUAL(fractal_get_height(myfractal),1000);
	CU_ASSERT_EQUAL((int)fractal_get_value(myfractal, 0,3),(int) 666);

	fractal_free(myfractal);
}

/*
*test le push et pop de l'object stack
*/
void testPushPopStack1(void) {

	fractal_t *fractal1 = (fractal_t*) malloc(sizeof(node_t));
	node_t * head = (node_t*) malloc(sizeof(node_t));
	head->fractal = fractal1;
	fractal_t *fractal2 = (fractal_t*) malloc(sizeof(node_t));
	pushInBuffer(&head, fractal2);
	CU_ASSERT_EQUAL(fractal2, head->fractal);
	CU_ASSERT_EQUAL(fractal1, head->next->fractal);
	popFromBuffer(&head);
	CU_ASSERT_EQUAL(NULL, head->next);
	popFromBuffer(&head);
	CU_ASSERT_EQUAL(NULL, head);

	free(fractal1);
	free(fractal2);
}


/*
* Tests the flush function of the stack
*/
void testFlushBuffer(void) {
	fractal_t *fractal1 = fractal_new("fractal1", 1000, 1000,1.0, 1.0);
	node_t * head = NULL;
	pushInBuffer(&head, fractal1);
	fractal_t *fractal2 = fractal_new("fractal2", 1000, 1000,1.0, 1.0);
	pushInBuffer(&head, fractal2);
	flushBuffer(&head);
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
   if (NULL == CU_add_test(pSuite, "test get et set de libfractal", testPushPopStack1)
  		||NULL == CU_add_test(pSuite, "test push et pop de stack", testGetterSetter)
			||NULL == CU_add_test(pSuite, "test si flushBuffer ne crash pas", testFlushBuffer))
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
