# How to Build.

Edit build.gradle and modify cFlags to point to your NDK directory.

Use AndroidStudio (tested on 1.2) to build the solution.

# Cocos Service Center (CSC) Dynamic JNITools

This project has utilities to make working with JNI more affordable.
While it has some specific code about Cocos Service Center, like a central Services registry,
it contains utilities to:

* call arbitrary static methods on any class.
* call arbitrary methods from a Service

These calls automatically infer the method signature based on the calling parameters. This infernce
happens at java side, where Boxed types are unboxed into primitives to figure out the best matching
signature.

It also offers a convenient message based mechanism to invoke a native callback from Java. This of
this as a java to native observer mechanism.

## Invoking java code from native

There are several methods available:

### Call a Service method

``` cpp
static jobject JavaDispatcher::callInService(
        const char* serviceCanonicalJavaClass,
        const char* methodName,
        jobjectArray args );

```

This method will invoke a method on a previously registered Service. The invocation does not need
a method signature and will be automatically inferred.
The <code>serviceCanonicalJavaClass</code> must identify a fully qualified Service class. These
Service must have been reflectively instantiated before at ServicesRegistry creation time.
<code>methodName</code> is a method from the Service. In future implementations of CSC, the
implementation will prevent from calling methods belonging to the Service interface, which are
handled by the Application and ServiceRegistry lifecycle.
<code>args</code> if a jobjectArray. Arguments are passed in to Java in its most polymorphic form.
Primitive arguments will be boxed. It is important to note that the implementation expects
primitive types in the Java side function. The boxed parameters will be automatically unboxed before
trying to find the suitable Java method signature for the calling parameters.

### Call an Arbitrary static method

``` cpp
static jobject JavaDispatcher::callStatic(
        const char* className,
        const char* methodName,
        jobjectArray args );
```

This method will invoke an arbitrary static method in any Class.
All the parameter definitions from the <code>call</code> method apply to this method as well.

In both cases, the result of the Java method invocation is returned. The return value will be a
jobject. In case the invoked method returned <code>void</code> or the method failed in any way
(from finding the class, to finding the method, to not finding a suitable method to invoke based
on the calling parameters).

### Call an Object instance method

```cpp
static jobject JavaDispatcher::callInInstance(
        jobject objRef,
        const char* method,
        jobjectArray params );
```

If you have an object instance reference in Native, you can call any method of such instance with
 this method. A Java instance can be obtained by using direct JNI calls, or by creating a
 <code>Proxy</code> object.

## Invoking native code from Java

This system is an observer pattern where native code registers interest in an Event (defined by a
String), and then Java can <code>emit</code> a notification back to any of the registered observers.

One remarkable feature of this approach is that you don't need to declare any native method in your Java classes. Everything is wrapped inside the NativeBridge class.

The process is therefore setup in two steps:

### Native

Native code registers interest in a Java event by calling:

``` cpp
NativeBridge::addEventListener( "GoogleAnalytics.Tick", functorCallback );
```

This code registers interest in having the <code>functorCallback</code> invoked whenever Java emits
an event identified by the registered interest ( "GoogleAnalytics.Tick" in this case ).

The functorCallback currently must be defined by an Object and a pointer to member.
The following is an example for <code>functorCallback</code>

``` cpp

class GA {
public:
   // functor parameters:
   // @param event the registered event. i.e.: "GoogleAnalytics.Tick".
   // @param params whatever object the java natvieEmit function call will pass in.
   void A( std::string event, jobject params ) { ... }
};

SPCallback* functorCallback= new SPCallback( new JNIEventListener<GA>( new GA(), &GA::A ) );

JNIEventListener extends Functor.
SPCallback is a shared_ptr<Functor>

// the SPCallback receives a Functor by simple polymorphism.

```

### Java

Java invocation just has to do calls of the form

``` java
servicesRegistry.nativeEmit( "GoogleAnalytics.Tick", object_param );
```

This call will invoke all native functors associated with this event.

## Helper methods

To assist in building JNI types, there's the JNIUtils class. It has methods to assist in building
the necessary <code>jobjectArray</code> for the <code>JavaDispatcher</code> calls as well as
construction/destruction for common types.
