Copyright (C) nemadesign GbR
$Rev: 80 $   
$Date: 2014-09-08 21:22:10 +0200 (Mo, 08. Sep 2014) $





Things to consider
==================
In case of a wrong linf for the 'Bilbiography' 
entry, add this before the '\addcontentsline{toc}{...}{...}:

Example:
-------- BEGIN ---------
\cleardoublepage
\phantomsection
% generate TOC
\addcontentsline{toc}{chapter}{Bibliography}
--------- END ----------

New macros
==========
\chapterpictopnum{_TEXT_}{_PIC_}{_WIDTH_}{_LABEL_}
\chapterpictopnonum{_TEXT_}{_PIC_}{_WIDTH_}{_LABEL_}

Add picture above chapter title

_TEXT_  : title for the chapter
_PIC_   : image file 
_WIDTH_ : relative width of text (1.0 equals text width)
_LABEL_ : name for the label of the chapter

Examples:
\chapterpicnum{Das Kapitel}{gfx/siemens-logo-wallpaper-1024x597.jpg}{0.5}
\chapterpicnonum{text text text}{gfx/siemens-logo-wallpaper-1024x597.jpg}{0.3}
______________________________________________________________________________________

\chapterpicrightnum{_TEXT_}{_PIC_}{_WIDTH_}{_LABEL_}
\chapterpicrightnonum{_TEXT_}{_PIC_}{_WIDTH_}{_LABEL_}

Add picture right to chapter title

_TEXT_  : title for the chapter
_PIC_   : image file 
_WIDTH_ : relative width of text (1.0 equals text width)
_LABEL_ : name for the label of the chapter

Examples:
\chapterpicrightnum{Chapter with Picture}{gfx/siemens-logo-wallpaper-1024x597.jpg}{0.5}
\chapterpicrightnonum{Chapter* with Picture}{gfx/siemens-logo-wallpaper-1024x597.jpg}{0.4}

______________________________________________________________________________________


\chapterpicteasernum{_TEXT_}{_PIC_}{_WIDTH_}{_LABEL_}{_TEASE_}
\chapterpicteasernonum{_TEXT_}{_PIC_}{_WIDTH_}{_LABEL_}{_TEASE_}

Chapter title with picture and teaser text.

In single column layout: white teaser text in grey box.
In double column layout: grey teaser text in white box.

_TEXT_  : title for the chapter
_PIC_   : image file 
_WIDTH_ : relative width of text (1.0 equals text width)
_LABEL_ : name for the label of the chapter
_TEASE_ : teaser text
Examples:
\chapterpicteasernum{Chapter \& Picture \& Teaser}{gfx/siemens-logo-wallpaper-1024x597.jpg}{0.5}{Text}
\chapterpicteasernonum{Chapter* \& Picture \& Teaser}{gfx/siemens-logo-wallpaper-1024x597.jpg}{0.4}{Text}

______________________________________________________________________________________

\tableofcontentspic{_TEXT_}{_PIC_}{_WIDTH_}
Add table of contents with picture

_TEXT_  : title for the chapter
_PIC_   : tmage file 
_WIDTH_ : relative width of text (1.0 equals text width)

Example:
\tableofcontentspic{Contents}{gfx/siemens-logo-wallpaper-1024x597.jpg}{0.5}

______________________________________________________________________________________

\gentitlepage{_PIC_}{_TEXT1_}{_TEXT2_}
Generate a titlepage

_PIC_    : image file (must be of square dimensions!)
_TEXT1_  : text above title
_TEXT2_  : text for title

Example:
\gentitlepage{gfx/Infineon-Baseband-Chip-X618.jpg}{Some text here}{Very long title for this Document Over Two Lines}
______________________________________________________________________________________

\gentitlepagenopic{_TEXT1_}{_TEXT2_}
Generate a titlepage without a picture

_TEXT1_  : text above title
_TEXT2_  : text for title

Example:
\gentitlepagenopic{Some text here}{Very long title for this Document Over Two Lines}
______________________________________________________________________________________

\genbackpage{_TEXT_}
Generate a backpage

_TEXT_  : white text

Example:
\genbackpage{Siemens AG}

______________________________________________________________________________________

\renewcommand{\shorttitle}{_TEXT_}
Set short title in header

_TEXT_  : text for short title

Example:
\renewcommand{\shorttitle}{Short title for paper}
______________________________________________________________________________________

\renewcommand{\shortdate}{_TEXT_}
Set date in short title

_TEXT_  : text for date in short title, default is \today

Example:
\renewcommand{\shortdate}{\today}
\renewcommand{\shortdate}{11.09.2011}
______________________________________________________________________________________

\renewcommand{\copyrightdate}{_TEXT_}
Set date in short title

_TEXT_  : text for date in copyright line on last page ,default is \today

Example:
\renewcommand{\copyrightdate}{\today}
\renewcommand{\copyrightdate}{11.09.2011}
______________________________________________________________________________________


Used LaTeX packages
===================
\usepackage[colorlinks=true, bookmarks=false]{hyperref}
\usepackage[scaled]{uarial}
\usepackage[T1]{fontenc}
%\usepackage[ngerman]{babel}
\usepackage[english]{babel}
\usepackage{amsmath}
\usepackage[pdftex]{graphicx}
\usepackage{multirow} 
\usepackage{subfigure} 
\usepackage{listings} 
\usepackage{courier}
\usepackage{titlesec}
\usepackage{scrpage2}
\usepackage{chngcntr}
\usepackage[table,usenames,dvipsnames]{xcolor}
\usepackage{booktabs}
\usepackage{color}
\usepackage{longtable}
\usepackage{colortbl}
\usepackage{fp}
\usepackage{eso-pic,picture}
\usepackage[absolute]{textpos}
\usepackage{multicol}
\usepackage{titletoc}
\usepackage{moresize}
\usepackage{printlen}
\usepackage{setspace}
\usepackage[colorlinks=true, bookmarks=false]{hyperref}

These packages are only used in the demo documents:
\usepackage{layout}
\usepackage{lipsum}


