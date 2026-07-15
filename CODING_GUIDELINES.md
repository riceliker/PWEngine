# The Guide Line for Code
Notice: This rule is use for C, C++, Zig, Rust file.

## 1. Basic 
Use space to retract, and a retract need eight space.
The `{` need in the next line, include Rust.
The label(include Switch Case, C++ Member Yield, Goto Label) `XXX:` never retract, but contains code must retract. 


## 2. Name (Although different language rule is not same)
Total: Do not use any abbreviation. Except the math or arithmetic inside(static) function
### 1. Variable
0. Use snake_case style.
1. Avoid global name. if can not avoid, use `PWL_` in head
2. If the type is bool, use `is_` in head.
3. Only number type can use `x y z w h r`. 

### 2. Function
0. Use BigCamel style
1. Export function name need use `PWL_` in head. Inside(static) function never use it.
2. If the function is function pointer, name as variable.
3. The first word in function must a verb. Like: `Get` `Set` `Find` `Submit` ...
4. The end word must tell whose struct. Use `In` will change struct variable. If not any word mean operation the instance.
Example: `PWL_SubmitSurfaceInScene` submit the surface in the Scene, the dictionary in scene will be add member. 

### 3. Class/Struct/Enum
0. Use BigCamel style
1. Export Struct/Enum name need use `PWL_` in head

### 4. Label
0. Use snake_case style.
1. When create something failed and use goto to clean, the label name must use `clean_` in the head.
2. When exit complex loop or fork by goto, the label name must use `exit_` in the head.

## 3. Function formal parameter
1. If the function is use for a struct, the first parameter must be a pointer for struct. And the name is `self`.
Example:
```C
        void PWL_SubmitSurfaceToScene(PWL_Scene* self, PWL_SurfacePool* pool, char* name);
                                                 ^ 
```
