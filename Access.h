#pragma once
#include <nan.h>
#include <string>
using namespace Nan;
using namespace v8;
using namespace std;

class Access : public ObjectWrap 
{
public:
	static void Init(Local<Object> exports);

private:
	explicit Access();
	~Access();

	static void New(const Nan::FunctionCallbackInfo<Value>& info);

	static void GetValue(const Nan::FunctionCallbackInfo<Value>& info);
	//static void GetValue(const Nan::FunctionCallbackInfo<v8::Value>& info);
	//static void PlusOne(const Nan::FunctionCallbackInfo<v8::Value>& info);
	//static void Multiply(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static Nan::Persistent<Function> constructor;
	wstring value{ L"Das ist aber schön" };
};

