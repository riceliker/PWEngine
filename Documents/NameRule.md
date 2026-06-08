# Name Rule 

## handle variable name rule

### nd wd
`nd`(need delete) mean it must be delete by function which input `wd`(will delete).
```C++
Object* nd_createObject(){}
void wd_deleteObject(Object* ptr){}

Object* nd_object_ptr = nd_createObject();
// Do Something
wd_deleteObject(nd_object_ptr);
```

### nds
`nds`(need delete by self) mean you should use `delete` to delete.
```C++
Object* nds_createObject(){}

Object* nds_object_ptr = nds_createObject();
// Do Something
delete nds_object_ptr;
``` 

### ad(default)
`ad`(auto delete) mean create object will auto delete(mean smart ptr or simple GC)
```C++
Object* ad_createObject(){}

Object* object_ptr = ad_createObject();
// Do Something
// Do not delete it.
```
