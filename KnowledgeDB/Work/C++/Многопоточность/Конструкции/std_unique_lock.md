Шаблон std::unique_lock обладает большей гибкостью, чем std:: lock_guard, потому что несколько ослабляет [[Инвариант|инварианты]] – экземпляр std::unique_lock не обязан владеть ассоциированным с ним [[Мьютекс(std_mutex)|мьютексом]]. Прежде всего, в качестве второго аргумента [[Конструктор|конструктору]] можно передавать не только объект std::adopt_lock, заставляющий объект управлять захватом [[Мьютекс(std_mutex)|мьютекса]], но и объект std::defer_lock, означающий, что в момент конструирования [[Мьютекс(std_mutex)|мьютекс]] не должен захватываться. Захватить его можно будет позже, вызвав функцию-член lock() объекта std::unique_lock (а не самого мьютекса) или передав функции std::lock() сам объект std::unique_lock.

```C++
class some_big_object;
void swap(some_big_object& lhs,some_big_object& rhs);
class X { 
private: 
	some_big_object some_detail;
	std::mutex m;
public: 
	X(some_big_object const& sd):some_detail(sd){} 
	friend void swap(X& lhs, X& rhs) { 
		if(&lhs==&rhs) return; 
		//std::defer_lock позволяет не захватывать мьютекс при создании объекта std::unique_lock.
		std::unique_lock lock_a(lhs.m,std::defer_lock); 
		std::unique_lock lock_b(rhs.m,std::defer_lock); 
		std::lock(lock_a,lock_b); // Захват двух мьютексов одновременно.
		swap(lhs.some_detail,rhs.some_detail);
	} };
```

std::unique_lock можно передавать функции std::lock() o, потому что в классе std::unique_lock имеются функции-члены lock(), try_lock() и unlock(). Для выполнения реальной работы они вызывают одноименные функции контролируемого [[Мьютекс(std_mutex)|мьютекса]], а сами только поднимают в экземпляре std::unique_lock флаг, показывающий, что в данный момент этот экземпляр владеет мьютексом. Флаг необходим для того, чтобы [[Деструктор|деструктор]] знал, вызывать ли функцию unlock(). Если экземпляр действительно владеет [[Мьютекс(std_mutex)|мьютексом]], то деструктор должен вызвать unlock(), в противном случае – не должен. Опросить состояние флага позволяет функция-член owns_lock(). Естественно, этот флаг необходимо где-то хранить. Поэтому размер объекта std::unique_lock обычно больше, чем объекта std:: lock_guard, и работает std::unique_lock чуть медленнее std:: lock_guard, потому что флаг нужно проверять и обновлять.

>std::unique_lock позволяет более гибко и безопасно захватывать и освобождать мьютексы. 

Данный механизм отлично подходит, когда необходимо уменьшить область захвата мьютекса:

```C++
void get_and_process_data() { 
//Во время работы process() захY ватывать мьютекс не нужно n std::unique_lock 
my_lock(the_mutex); 
some_class data_to_process=get_next_data_chunk(); 
my_lock.unlock(); 
result_type result=process(data_to_process); 
my_lock.lock(); 
//Снова захватить мьютекс перед записью результатов 
write_result(data_to_process,result);
}
```

>В общем случае блокировку следует удерживать ровно столько времени, сколько необходимо для завершения требуемых операций.