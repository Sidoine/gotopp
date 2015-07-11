#!/usr/bin/perl

my $base=$ARGV[0];

open(F,'src/ass.txt') || die "Impossible de trouver ass.txt";
while ($l=<F>)
{
	if ($l=~/^\$(\w*)/)
	{
		$ncmd=$1;
		
		$cmd=$ncmd;
	}
	elsif ($l=~s/^(\w+)=//)
	{
		if ($1 eq 'desc')
		{
			$desc{$cmd}.=$l;
			#print "$cmd=$l\n";
		}
		elsif ($1 eq 'pile')
		{
			$pile{$cmd}.=$l;
		}
	}
}
close(F);

undef $/;
open(F,'../interpreteur/princ/desass.cpp') || die "Impossible de trouver desass.cpp";
$truc=<F>;
close(F);
$/="\n";
$truc=~/Code\[\]=\s+\{\s*(.*?)\s*\};/s;
$chose=$1;
@chose=split(/\n/,$chose);

open(F,'>$base/chm/html/Code+machine.html');
print F <<"ENTETE";
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<title>Assembleur GOTO++</title>
	<link rel="stylesheet" type="text/css" href="style.css" />
</head>
<body bgcolor="#FFFFFF">
<h1>Codes de la machine virtuelle GOTO++</h1>
<h2>Introduction</h2>
<p>Le code GOTO++ est une suite d'instructions constituées d'un opérateur
sur un octet suivi de paramètres. Ces paramètres peuvent être de différents
types (le nombre entre parenthèses est le numéro du type) :</p>
<p>Inconnu (0) : valeur indéfinie.</p>
<p>Fonction (1) : un entier sur 4 octets indiquant un numéro de fonction
incorporée au compilateur.</p>
<p>Chances (2) : un flottant sur 4 octets.</p>
<p>Caractère (3) : un caractère sur 1 octet.</p>
<p>Variable (4) : un numéro de variable sur 4 octets.</p>
<p>Entier (5) : un entier sur 4 octets.</p>
<p>Type (6) : un numéro de type de variable, sur 1 octet.</p>
<p>Code (7) : réservé.</p>
<p>Chaîne (8) : un entier sur 4 octets valant 1, suivi d'un entier sur
4 octets indiquant la longueur de la chaîne en octets, et enfin la chaîne, terminée
par un caractère nul. Pour l'instant chaque caractère tient sur un octet.</p>
<p>Fichier (9) : réservé.</p>
<p>Tableau (10) : réservé.</p>
<p>Référence (11) : réservé.</p>
<p>Etiquette (12) : un entier sur 4 octets, ciblant un bout de code, en 
nombre d'octets à partir du début du code.</p>
<p>DebutParam (13) : correspond au début des paramètres d'une fonction et n'a pas de valeurs.</p>
<p>VariableLoc (14) : un numéro de variable locale sur 4 octets.</p>
<p>EtiqLoc (15) : réservé, utiliser le type Etiquette.</p>
<p>Hachage (16) : réservé.</p>
<p>Classe (17) : numéro de classe sur 4 octets.</p>
<p>EtiqAuto (18) : réservé.</p>
<p>Bébé (19) : réservé.</p>
<p>ObjetC (20) : réservé.</p>
<p>FonctionModule (21) : un entier sur 4 octets, indiquant un numéro de fonction de module.</p>
<p>EtiqAutoA (22) : réservé.</p>
<p>ExpReg (23) : 4 octets valant 1, 4 octets valant la longueur de l'expression régulière (y compris ces 8 premiers octets) puis l'expression régulière.</p>
<p>Entier64 (24) : un entier sur 8 octets.</p>
<p><b>Remarque</b> : la mention « réservé » signifie que ces variables ne peuvent
pas être inclues dans le code.</p>


<h2>Liste des codes</h2>
ENTETE

%type=('TYPE_ENTIER' => 'entier',
	'TYPE_CHANCES' => 'chances',
	'TYPE_CHAINE' => 'chaine',
	'TYPE_INCONNU' => 'inconnu',
	'TYPE_TYPE' => 'type',
	'TYPE_VARIABLE' => 'numéro de variable globale',
	'TYPE_VARIABLELOC' => 'numéro de variable locale',
	'TYPE_CLASSE' => 'numéro de classe');

$n=0;
foreach $i (@chose)
{
#print $i,"\n";
	if ($i!~/"(.*?)"(?:,\s*(\d+))?(?:,\s*\{\s*(.*?)\s*\})?/)
	{
		die "$i au mauvais format";
	}
	#print $1;
	$nom=$1;
	$nombre=$2;
	$type=$3;
	@type=split(/\s*,\s*/,$type);
	
	push(@liste,$nom);
	print F <<"TRUC";
<a name="$nom" /><h3>$n - $nom</h3>
<p><b>Opérandes :</b> 
TRUC
	if (scalar @type)
	{
		$z=0;
		foreach $t (@type)
		{
			print F ', ' if ($z);
			$z++;
			print F "$type{$t}$z";
		}
	}
	else
	{
		print F 'aucun';
	}
	print F '</p>';
	if ($pile{$nom})
	{
		print F "<p><b>Paramètres de la pile :</b> $pile{$nom}</p>";
	}
	print F "<p>$desc{$nom}</p>";
	$n++;
}

print F "<h2>Liste alphabétique</h2><table>";
$n=0;
@liste=sort(@liste);
foreach $l (@liste)
{
	if ($n % 4 ==0)
	{
		print F '<tr>';
	}
	print F "<td><a href=\"#$l\">$l</td>\n";
	if ($n % 4 ==3)
	{
		print F '</tr>';
	}
	$n++;
}
print F '</table></body></html>';
close(F);
