
### Warning: This experimental project is under heavy development. Do not expect this project to be production-ready as long as this text is still here.

# What is Seamless?


![Seamless Logo][logo]

Seamless (CMLS) is a brand-spanking new experimental web framework. It includes:

* A C++20 template engine that allows you to write C++ and markup in the same file, just like you would with Laravel's blade engine for PHP, or ASP.NET Razor. The file extension for these files is `.cmls`.
* Automatically detects file changes and recompiles your views (.cmls files) in record speeds. This happens on-demand on new requests, which makes it very reliable.\*
* A simple C++ module system to more easily use external libraries and support this project.
* A base C++ library (the `Seamless` module) that provides the high-level functionality that you've come to expect from a high-level project like this.
* Native server plugins for IIS on Windows, and Apache or Nginx on Linux.
* A C++ compiler frontend that supports MSBuild on Windows, and GCC or clang on Linux.
* An easy-to-use ORM with support for multiple database backends.
* ***(NEW)*** Plans to implement complementary and integrated JS framework for Single-Page Applications and similiar uses.

\* *During production, it is recommended that on-demand recompilation is turned off. Not only does this further increase performance, it's also prevents a huge security issue. Turning it off additionally allows you to skip the installation of build tools on the production server.*

*Note: see feature completion below.*

---

## Example .cmls files

A view in Seamless consists of a .cmls file, which in turn may resolve other .cmls files (to *extend* them like you would in Laravel).

An example with a main layout view, and an *article* view that resolves the layout before implementing it.

`layout.cmls`
```cmls
@// Import the module containing your models, controllers etc.
@import MySite

<html>
    <head>
        <title>My Site - @virtual(title)</title>
        
        <link rel="stylesheet" type="text/css" href="static/css/style.css" />
        @virtual(styles)

        <script type="text/javascript" src="static/js/jquery.js" defer></script>
        @virtual(scripts)

    </head>
    <body>
        @virtual(content)
    </body>
</html>
```

`sub/article.cmls`
```cmls
@resolve "layout.cmls"

@parameter MySite::Article* article

@push scripts <script type="text/javascript" src="static/js/article.js" defer></script>
@push styles <link rel="stylesheet" type="text/css" href="static/css/article.css" />

@implement title {{ article->title() }}

@implement content
    <article>
        <h2>{{ article->title() }}</h2>
        <p>{{ article->body() }}</p>
    </article>

    <div>
        @for(auto comment : article->comments())

            <@c++ auto user = comment->user(); @>

            <div>
                <h4><a href="users/{{ user->id() }}">{{ user->name() }}</a></h4>
                <p>{{ comment->body() }}</p>
            </div>

        @end
    </div>
@done
```

---

## Why a new web framework?

*Warning: Personal opinion below.*

Modern web development is very bloated these days. The sheer amount of tools, libraries, frameworks and different technologies available today is staggering, so you'd think that someone who wants to add a *new* framework to the pile is crazy.

However, the pile of available web technology has slowly been growing upon itself for the past decades, which has in turn resulted in layers upon layers of unneccessary and redundant abstractions that bloats the entire web ecosystem.

This project attempts to cut a big bunch of these abstractions and bloat away, by implementing a *native* framework with modern C++, along with well-supported *native* server modules for popular server infrastructure. It aims to inject stability and competence back into the world of web development.

When technologies like PHP and ASP were invented, they fulfilled a need to bring programming in to the world of web development. Using something native back then (like C or C++89) was simply too much effort for very little gain. Today though, 28 years after the birth of PHP, C++ has evolved immensely, and the continued need for higher-level abstractions for web development simply isn't there anymore. Native programming is ready for web development (and not via CGI this time).

---

## How does it work?

If you've changed a `.cmls` template file, and someone then tries to browse to a page where this template is used, it is automatically recompiled and reloaded on-the-fly. This happens in about 1 second on my machine for most views, with optimizations turned on, which usually increase the compilation time.

Views are first compiled into C++ by the template compiler. The resulting source is then compiled by a C++ compiler into a system-targeted dynamic module (`.dll`/`.so`), which in turn is then (re-)loaded by the view manager(`cmls::ViewManager`).

This means that, both during production and development, the web server handles all requests natively, all the way. The web server passes a request in to the Seamless *Application*, which in turn passes it to a *Controller*. The *Controller* may then invoke a *View* if it wants, which basically just calls a function in the compiled view module (`.dll`/`.so`). The generated response is then passed back all the way to the web server, which does it's thing to pass it to the client.

**During production, it is recommended that on-demand recompilation is turned off. Not only does this further increase performance, it's also a huge security issue. Turning it off additionally allows you to skip the installation of build tools on the production server.**

---

## What is working as of today?

As stated, the framework is under heavy development. As such, many features will be broken, or even just straight up missing. Below follows a list of features and their current development status.

<dl>
  <dt><strong>Missing</strong></dt>
  <dd>This feature is straight up missing. Have patience, they're coming!</dd>

  <dt><strong>In progress</strong></dt>
  <dd>Work on this progress has begun, however it's considered broken until status becomes <em>Working</em>. You are ill-advised to use these features.</dd>

  <dt><strong>Done</strong></dt>
  <dd>This feature has base functionality working, however it's considered unstable until status becomes <em>stable</em>. You are advised to use caution when using these features, and be prepared for refactors and other changes.</dd>

  <dt><strong>Stable</strong></dt>
  <dd>This feature has been around for a while and is considered stable and mature. You could probably use this feature alone in production, although as an open-source project I can not leave any guarantees!</dd>
</dl>

| Feature                                | Status       |
| -------------------------------------- |-------------:|
| Base library                           | Stable       |
| CMLS Template Compiler                 | Done         |
| Automatic on-demand recompilation      | Done         |
| C++ Module System                      | Done         |
| C++ Compiler (MSBuild/Win64)           | Done         |
| Router and Controller system           | Done         |
| Server Module (IIS/Win64)              | Done         |
| Server Module (Apache/Linux)           | In progress  |
| Server Module (Nginx/Linux)            | Missing      |
| C++ Compiler (GCC/Linux)               | Missing      |
| C++ Compiler (Clang/Linux)             | Missing      | 
| ORM system                             | Missing      | 

---

## Documentation

@todo

[logo]: ./Design/LogoSmall.png "Seamless Logo"
[perf]: ./Design/LogoSmall.png "Performance"