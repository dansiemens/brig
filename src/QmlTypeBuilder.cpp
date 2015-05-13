#include <unistd.h>
#include <node.h>
#include <QObject>
#include "QmlTypeBuilder.h"

namespace Brig {

	using namespace v8;
	using namespace node;

	Persistent<Function> QmlTypeBuilder::constructor;

	QmlTypeBuilder::QmlTypeBuilder(const char *typeName) : ObjectWrap()
	{
		metaobject_builder = new DynamicQMetaObjectBuilder(typeName);
		obj = NULL;
	}

	QmlTypeBuilder::~QmlTypeBuilder()
	{
		delete metaobject_builder;
	}

	void QmlTypeBuilder::Initialize(Handle<Object> target)
	{
		HandleScope scope;

		Local<String> name = String::NewSymbol("QmlTypeBuilder");

		/* Constructor template */
		Persistent<FunctionTemplate> tpl = Persistent<FunctionTemplate>::New(FunctionTemplate::New(QmlTypeBuilder::New));
		tpl->InstanceTemplate()->SetInternalFieldCount(1);  
		tpl->SetClassName(name);

		/* Prototype */
		NODE_SET_PROTOTYPE_METHOD(tpl, "addSignal", QmlTypeBuilder::addSignal);
		NODE_SET_PROTOTYPE_METHOD(tpl, "build", QmlTypeBuilder::build);

		constructor = Persistent<Function>::New(tpl->GetFunction());

		target->Set(name, constructor);
	}

	Handle<Value> QmlTypeBuilder::New(const Arguments& args)
	{
		HandleScope scope;

		String::Utf8Value typeName(args[0]->ToString());

		QmlTypeBuilder *obj_wrap = new QmlTypeBuilder(*typeName);
		obj_wrap->Wrap(args.This());

		return args.This();
	}

	Handle<Value> QmlTypeBuilder::build(const Arguments& args)
	{
		HandleScope scope;

		QmlTypeBuilder *qmltype_builder = ObjectWrap::Unwrap<QmlTypeBuilder>(args.This());

		// Generate a new QMetaObject
		QMetaObject *metaobject = qmltype_builder->metaobject_builder->build();

		// Create a new QObject
		qmltype_builder->obj = new DynamicQObject(metaobject);

		return Undefined();
	}

	Handle<Value> QmlTypeBuilder::addSignal(const Arguments& args)
	{
		HandleScope scope;

		QmlTypeBuilder *qmltype_builder = ObjectWrap::Unwrap<QmlTypeBuilder>(args.This());

		String::Utf8Value signature(args[0]->ToString());

		QStringList parameterNames;

		qmltype_builder->metaobject_builder->addSignal(*signature, parameterNames, args[2]);

		return Undefined();
	}
}