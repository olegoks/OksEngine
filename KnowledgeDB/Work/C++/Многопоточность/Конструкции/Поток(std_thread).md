Для запуска [[Поток|потока]] следует сконструировать объект std::thread, который определяет, какая задача будет выполняться на потоке. В роли задачи может выступать функция или объект-функция. Эта функция работает в свое потоке пока не вернет управление и в этот момент поток завершается.
```C++
void DoSomeWork();
std::thread myThread(DoSomeWork);
```
Класс std::thread работает с любым типом, допускающим вызов(Callable), поэтому конструктору std::thread можно передать экземпляр класса, в котором определен оператор вызова:
```C++
class BackgroundTask {
public:
	void operator() const {
	DoSomething();
	}
};
BackgroundTask task;
std::thread myThread(task);
```

В данном случае переданные объект-функция копируется в память, принадлежащую только что созданному потоку выполнения, и оттуда вызывается. Поэтому необходимо, чтобы копия была эквивалентная оригиналу, иначе можно получить неожиданный результат. При передаче объекта функции [[Конструктор|конструктору]] потока необходимо избежать феномена [[Most vexing parse]].
После запуска потока необходимо решить ждать его завершения или предоставить собственной судьбе. До вызова [[Деструктор|деструктора]] объекта std::thread необходимо вызвать join() или detach() в противном случае деструктор std::thread вызовет std::terminate. 

Передача аргументов вызываемому объекту или функции сводится просто к передаче дополнительных аргументов конструктору std::thread, по умолчанию эти аргументы копируются в память объекта. Причем так происходит даже в том случае, когда функция ожидает на месте соответствующего параметра ссылку:
```C++
void f(int i,std::string const& s);
std::thread t(f,3,”hello”);
```
В данном случае преобразование const char* к типу std::string происходит в контексте нового потока. Это особенно важно когда переданный аргумент является указателем на автоматическую переменную:
```C++
void f(int i,std::string const& s);

void oops(int some_param) { 
	char buffer[1024]; 
	sprintf(buffer,”%i”,some_param);
	std::thread t(f,3,buffer); 
	t.detach(); 
}
```

В данном примере в новый поток передается указатель на локальную переменную buffer и есть шансы, что выход из функции oops произойдет раньше чем буфер будет преобразован к типу std::string в новом потоке, в таком случае мы получим [[Неопределенное поведение|неопределенное поведение]]. Решение заключается в том, чтобы выполнить преобразование до передачи buffer конструктору std::thread:
```C++
void f(int i,std::string const& s);
void not_oops(int some_param) { 
	char buffer[1024]; 
	sprintf(buffer,”%i”,some_param);
	std::thread t(f,3,std::string(buffer));
}
```

Возможен и обратный сценарий, когда мы хотим передать ссылку на объект, который хотим модифицировать, а в итоге поток делает копию данных, делает там изменения и по окончанию работы удаляет эти данные:

```C++
void update_data_for_widget(widget_id w,widget_data& data); 

void oops_again(widget_id w) { 
	widget_data data;
	std::thread t(update_data_for_widget,w,data); 
	display_status(); 
	t.join(); 
	process_widget_data(data);
}
```
Для решения ситуации нужно обернуть аргументы, которые должны быть ссылками, объектом std::ref:
```C++
std::thread t(update_data_for_widget,w,std::ref(data));
```

Передача функции-члена в качестве в качестве функции исполнения:
```C++
class X { 
public: void do_lengthy_work();
}; 
X my_x;
std::thread t(&X::do_lengthy_work,&my_x);
```

Передача владения потоком от одного объекта std::thread другому:

```C++
void some_function();
void some_other_function();
std::thread t1(some_function); 
std::thread t2=std::move(t1); 
t1=std::thread(some_other_function); 
std::thread t3; 
t3=std::move(t2); 
t1=std::move(t3); // Это приводит к аварийному завершению программы.
```

Возврат объекта из функции:
```C++
std::thread f() { 
void some_function();
return std::thread(some_function); }
```

Передача объекта потока в функцию:
```C++
void f(std::thread t);
void g() { 
	void some_function();
	f(std::thread(some_function));
	std::thread t(some_function);
	f(std::move(t));
}
```

Идентификатор потока имеет тип std::thread::id. 