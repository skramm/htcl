# htcl: Html Tag CLoud generator

* language: c++11
* status: beta

### Use case ###
Say you have some set of resource files (pdf, html pages, whatever), each of them describing a few topics.
This set of files is referenced as links on a main "index" html pages. Fine.

But what if a user wants to find and/or read all the files where a given topic is referenced ?

This tool will generate from amanually written input csv file two small html snippets that you can insert into the index html page.

The two output files will be:
 * `htcl_cloud.html`: holds a cloud tag, as a set of internal links on the file links below
 * `htcl_links.html`: holds as an html list a set of links on "real" files, for each tag

Beware: these two code snippets must eventually be in the same html file.

 Of course, the user must provide the input csv file, that holds as one line pre file all the related tags.
 Each line holds three fields:
  1. the location (path to the file)
  1. the file name
  1. the tags, space separated

 As an example, consider this input csv file:
```
folder1;file1.pdf;c++ python java
folder1;file2.pdf;python java shell
folder1;file3.pdf;basic perl java
folder2;file1.pdf;perl
```
This will produce these two files:

htcl_cloud.html:

```
<div id='tag_cloud'>
<a href='#t_basic'>basic</a>
<a href='#t_c++'>c++</a>
<a href='#t_java'>java</a>
<a href='#t_perl'>perl</a>
<a href='#t_python'>python</a>
<a href='#t_shell'>shell</a>
</div>
```

htcl_links.html:
```
<div id='tag_links'>
<ul>
<li id='t_basic'>
basic: <a href='folder1/file3'>file3</a>
</li>
<li id='t_c++'>
c++: <a href='folder1/file1'>file1</a>
</li>
<li id='t_java'>
java: <a href='folder1/file1'>file1</a>
<a href='folder1/file2'>file2</a>
<a href='folder1/file3'>file3</a>
</li>
<li id='t_perl'>
perl: <a href='folder1/file3'>file3</a>
<a href='folder2/file1'>file1</a>
</li>
<li id='t_python'>
python: <a href='folder1/file1'>file1</a>
<a href='folder1/file2'>file2</a>
</li>
<li id='t_shell'>
shell: <a href='folder1/file2'>file2</a>
</li>
</ul></div>
```
