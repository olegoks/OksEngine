[std::call_once - cppreference.com](https://en.cppreference.com/w/cpp/thread/call_once)
[std::once_flag - cppreference.com](https://en.cppreference.com/w/cpp/thread/once_flag)

Случай [[Защита разделяемых данных во время инициализации|защиты разхделяемых данных во время инициализации]] встречается крайне часто, поэтому комитет по стандартизации счел важным включить специальный механизм.

Вместо того чтобы захватывать [[Мьютекс(std_mutex)|мьютекс]] и явно проверять указатель, каждый [[Поток|поток]] может просто вызвать функцию std::call_once, твердо зная, что к моменту возврата из нее указатель уже инициализирован каким-то потоком (без нарушения синхронизации). Обычно издержки, сопряженные с использованием std::call_once, ниже, чем при явном применении мьютекса, поэтому такое решение следует предпочесть во всех случаях, когда оно не противоречит требованиям задачи. В данном случае инициализация производится путем вызова функции, но ничто не мешает завести для той же цели класс, в котором определен оператор вызова. Как и большинство функций в стандартной библиотеке, принимающих в качестве аргументов функции или предикаты, std::call_once работает как с функциями, так и с объектами, допускающими вызов.

```C++
#include <mutex>
std::shared_ptr resource_ptr;
std::once_flag resource_flag;
void init_resource() { 
	resource_ptr.reset(new some_resource);
} 
void foo() { 
	std::call_once(resource_flag,init_resource);
	resource_ptr->do_something();
}
```

Здесь переменная типа std::once_flag n и инициализируемый объект определены в области видимости пространства имен, но std:: call_once() вполне можно использовать и для [[Отложенная инициализация|отложенной инициализации]] членов класса, как показано в следующем листинге:

```C++
class X { 
private: 
	connection_info connection_details;
	connection_handle connection;
	std::once_flag connection_init_flag;
void open_connection() { 
	connection = connection_manager.open(connection_details);
} 
public: 
	X(connection_info const& connection_details_):
		connection_details(connection_details_) { } 
		
void send_data(data_packet const& data) { // 1
	std::call_once(connection_init_flag,&X::open_connection,this); // 2
	connection.send_data(data); 
} 
data_packet receive_data() { // 3
	std::call_once(connection_init_flag,&X::open_connection,this); // 2
	return connection.receive_data(); 
} 
};
```

В этом примере инициализация производится либо при первом обращении к send_data() (1), либо при первом обращении к receive_ data() (3). Поскольку данные инициализируются функцией-членом open_connection(), то требуется передавать также указатель this. Как и во всех функциях из стандартной библиотеки, которые принимают объекты, допускающие вызов, (например, конструктор std:: thread и функция std::bind()), это делается путем передачи std:: call_once() дополнительного аргумента (2). 
Следует отметить, что, как и в случае std::mutex, объекты типа std::once_flag нельзя ни копировать, ни перемещать, поэтому, если вы собираетесь использовать их как члены классы, то соответствующие [[Конструктор|конструкторы]] придется определить явно (если это необходимо).
