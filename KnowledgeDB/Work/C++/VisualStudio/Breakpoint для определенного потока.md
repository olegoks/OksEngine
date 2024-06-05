![[Pasted image 20240605112926.png]]

Изменить название потока можно с помощью такого кода:

```C++
HRESULT r;
r = SetThreadDescription(
	GetCurrentThread(),
	L"Resource system loop thread"
	);
```