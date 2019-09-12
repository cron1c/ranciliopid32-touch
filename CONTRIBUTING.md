# Contributing Guideline

**Do you have some free time to spend with programming? You like Coffee and you know about PID or other stuff? See how can you help to improve the graphics library!**

There are many ways to join the community. If you have some time to work with us I'm sure you will find something that fits you! You can:
-   help others in the [Forum](https://forum.ranciliopid.de/)
-   create/improve and/or translate the documentation
-   write a blog post about your experiences
-   report and/or fix bugs
-   suggest and/or implement new features

But first, start with the most Frequently Asked Questions.

## Where can I write my question and remarks?

We use the [Forum](https://forum.ranciliopid.de/) and [Chat](https://chat.rancilio-pid.de/) to ask and answer questions and [GitHub's issue tracker](https://github.com/cron1c/ranciliopid32-touch/issues) for development-related discussion.

But there are some simple rules:

-   Be kind and friendly.
-   Speak about one thing in one issue/topic.
-   Give feedback and close the issue or mark the topic as solved if your question is answered.
-   Tell what you experience or expect. _"The button is not working"_ is not enough info to get help.
-   If possible send an absolute minimal code example in order to reproduce the issue
-   Use [Markdown](https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet) to format your post.

## How can I send fixes and improvements?

erging new code happens via Pull Requests. If you are still not familiar with the Pull Requests (PR for short) here is a quick guide:

1.  **Fork** the [rancilio pid repository](https://github.com/cron1c/ranciliopid32-touch). To do this click the "Fork" button in the top right corner. It will "copy" the `ranciliopid` repository to your GitHub account (`https://github.com/your_name?tab=repositories`)
2.  **Clone** the forked repository and add your changes
3.  **Create a PR** on GitHub from the page of your `ranciliopid` repository (`https://github.com/your_name/ranciliopid`) by hitting the "New pull request" button
4.  **Set the base branch**. It means where you want to merge your update. Fixes go to `master`, new features to the actual `dev-x.y or beta-x.y` branch.
5.  **Describe** what is in the update. An example code is welcome if applicable.

## Coding guide

-   Variables:
    -   One line, one declaration (BAD: char x, y;)
    -   Use `<stdint.h>` (_uint8_t_, _int32_t_ etc)
    -   Declare variables when needed (not all at function start)
    -   Use the smallest required scope
    -   Variables in a file (outside functions) are always _static_
    -   Do not use global variables or try to avoid them (use functions to set/get static variables)
    
  -  Functions:
	  - Make use of the config file. Every new feature should have the option to enable or disable via the config file.
	  - Every function should support every userconfiguration (pidonly,display type, brewdetection,with and without touch+event handler)
	  - Delay is not allowed. 
	  - Use /* */ for comments above a new area of code. Use // to describe a line of code. 
    
    
    to be continued..
    
    

