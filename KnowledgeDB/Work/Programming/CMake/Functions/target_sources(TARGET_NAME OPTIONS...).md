## Description
>The function accepts as the first argument the name of the target. The following argument is a declaration of how the property should be propagated to other targets that depend on the target `my_exec`. Valid accessor values are:
- **PUBLIC**: the property gets forwarded to targets which depend on the current target.
- **PRIVATE**: the property is used for the current target and is forwarded to other targets which depend on the current target
- **INTERFACE**: the property is not used to build the current target, but is forwarded to other targets which depend on the current target
## Parameters
* 
## Example of using
```cmake
target_sources( my_exec PRIVATE source1.f90 source2.f90 )
```


