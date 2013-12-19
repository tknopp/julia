#include <julia.h>
#include <stdio.h>
#include <math.h>

//#include<pthread.h>
pthread_mutex_t lock;

double my_c_sqrt(double x)
{
    return sqrt(x);
}

int main()
{
      if (pthread_mutex_init(&jl_lock, NULL) != 0)
      {
        printf("\n mutex init failed\n");
        return 1;
      }

      if (pthread_mutex_init(&lock, NULL) != 0)
      {
        printf("\n mutex init failed\n");
        return 1;
      }

    jl_init(NULL);

    {
      // Simple running Julia code

      jl_eval_string("println(sqrt(2.0))");
    }

    {
      // Accessing the return value

      jl_value_t *ret = jl_eval_string("sqrt(2.0)");

      if(jl_is_float64(ret))
      {
        double retDouble = jl_unbox_float64(ret);
        printf("sqrt(2.0) in C: %e\n", retDouble);
      }
    }

    {
      // Same as above but with function handle (more flexible)

      jl_function_t *func = jl_get_function(jl_base_module, "sqrt");
      jl_value_t* argument = jl_box_float64(2.0);
      jl_value_t* ret = jl_call1(func, argument);

      if(jl_is_float64(ret))
      {
        double retDouble = jl_unbox_float64(ret);
        printf("sqrt(2.0) in C: %e\n", retDouble);
      }
    }

    {
      // 1D arrays

      jl_value_t* array_type = jl_apply_array_type( jl_float64_type, 1 );
      jl_array_t* x          = jl_alloc_array_1d(array_type , 10);
      JL_GC_PUSH1(&x);

      double* xData = jl_array_data(x);
size_t i;
      for(i=0; i<jl_array_len(x); i++)
        xData[i] = i;

      jl_function_t *func  = jl_get_function(jl_base_module, "reverse!");
      jl_call1(func, (jl_value_t*) x);

      printf("x = [");
      for(size_t i=0; i<jl_array_len(x); i++)
        printf("%e ", xData[i]);
      printf("]\n");

      JL_GC_POP();
    }

    {
      // define julia function and call it

      jl_eval_string("my_func(x) = 2*x");

      jl_function_t *func = jl_get_function(jl_current_module, "my_func");
      jl_value_t* arg = jl_box_float64(5.0);
      double ret = jl_unbox_float64(jl_call1(func, arg));

      printf("my_func(5.0) = %f\n", ret);
    }

    {
      // call c function 

      jl_eval_string("println( ccall( :my_c_sqrt, Float64, (Float64,), 2.0 ) )");
    }

    {
      // check for exceptions

      jl_eval_string("this_function_does_not_exist()");

      if (jl_exception_occurred())
          printf("%s \n", jl_get_exception_str( jl_exception_occurred() ) );
    }



    
    {
      // define julia function and call it






      jl_function_t *func = (jl_function_t*) jl_eval_string("my_func(x) = sin( norm(2*x*ones(30)) )");

//      jl_function_t *func = jl_get_function(jl_current_module, "sin");
      int i;
//jl_gc_disable();
      jl_value_t* arg = jl_box_float64((double)0);
      //pthread_mutex_lock(&lock);
      jl_call1(func, arg);

      //func = jl_get_specialization(func, jl_tuple1(jl_float64_type));
      //jl_function_ptr(func, jl_float64_type, jl_tuple1(jl_float64_type));

      //
      #pragma omp parallel for
      for(i=0;i < 4; i++)
      {

        jl_value_t* arg = jl_box_float64((double)i);
        //pthread_mutex_lock(&lock);
        //jl_value_t* retJ = jl_apply(func, &arg, 1);
        jl_value_t* retJ = jl_call1(func, arg);
        //pthread_mutex_unlock(&lock);
        double ret = jl_unbox_float64(retJ);

        printf("my_func() = %f\n", ret);
      }

    pthread_mutex_destroy(&jl_lock);
pthread_mutex_destroy(&lock);
    }

    return 0;
}


