
Etape 1: construction d'une table de vérité (mannuellement)
avec l'appli `misc/figures_src/src/corner_side_2.cpp`

Etape2:
A partir de la table de vérité donnant les croisements:
```
string	cross	decimal
000001	y	1
000010	y	2
000100	y	4
000101	y	5
001000	y	8
001010	y	10
010010	y	18
010100	y	20
010110	y	22
011000	y	24
011001	y	25
011110	y	30
100001	y	33
100010	y	34
100110	y	38
100111	y	39
101001	y	41
101011	y	43
110101	y	53
110111	y	55
111010	y	58
111011	y	59
111101	y	61
111110	y	62
```
On extrait l'équation

Manuellement:

y=/a/b(/c /d /e f + /c /d e /f + 

construction du mot binaire (6 bits):

(ordre: poids fort -- poids faible)
```
s12
s21
side( ptA, li1 )==-1?0:1
side( ptB, li1 )==-1?0:1
side( ptA, li2 )==-1?0:1
side( ptB, li2 )==-1?0:1
```


Simplification:

Using:http://www.32x8.com/

method:Quine-McCluskey Method (SOP)

y = A'B'C'E'F + B'C'D'E'F + A'B'D'EF' + A'C'DE'F' + A'B'CD'F' + A'BC'EF' + A'BCD'E' + BCDEF' + AB'C'EF' + AC'DEF + AB'CD'F + ABDE'F + ABCD'E

method: SOP

y = AB'C'EF' + AB'CD'F + A'BC'DF' + A'C'D'EF' + ABDE'F + B'C'D'E'F + AC'DEF + BCDEF' + A'BCD'E' + A'B'CD'F' + ABCD'E + A'B'C'DE'
