#include <julia.h>
#include <stdio.h>
#include <math.h>

jl_value_t* my_c_sqrt(jl_value_t* F, jl_value_t** args, uint32_t nargs)
// alternatively: JL_CALLABLE(my_c_sqrt)
{
    JL_NARGS(my_c_sqrt,1,1);
    JL_TYPECHK(my_c_sqrt, float64, args[0])

    double x = jl_unbox_float64(args[0]);
    x = sqrt(x);
    return jl_box_float64(x);
}

int main()
{
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

      jl_sym_t* sym = jl_symbol("sqrt");
      jl_function_t *func = (jl_function_t*) jl_get_global(jl_base_module, sym);
      jl_value_t* argument = jl_box_float64(2.0);
      jl_value_t* ret = jl_apply(func, &argument , 1);

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

      double* xData = jl_array_data(x);

      for(size_t i=0; i<jl_array_len(x); i++)
        xData[i] = i;

      jl_sym_t* sym        = jl_symbol("reverse!");
      jl_function_t *func  = (jl_function_t*) jl_get_global(jl_base_module, sym);
      jl_apply(func, (jl_value_t **) &x , 1);

      printf("x = [");
      for(size_t i=0; i<jl_array_len(x); i++)
        printf("%e ", xData[i]);
      printf("]\n");

    }

    {
      // define julia function and call it

      jl_eval_string("my_func(x) = 2*x");

      jl_function_t *func = (jl_function_t*)jl_get_global(jl_current_module, jl_symbol("my_func"));
      jl_value_t* arg = jl_box_float64(5.0);
      double ret = jl_unbox_float64(jl_apply(func, &arg, 1));

      printf("my_func(5.0) = %f\n", ret);
    }

    {
      // register and call c function

      jl_sym_t* name = jl_symbol("my_c_sqrt");
      jl_set_const(jl_current_module,
                   name,
                   (jl_value_t*) jl_new_closure(my_c_sqrt, (jl_value_t*) name, NULL)
                   );

      jl_eval_string("println( my_c_sqrt(2.0) )");
    }

    return 0;
}


