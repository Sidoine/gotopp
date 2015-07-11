#!/usr/bin/perl
use File::Copy;

my $base='.';
#Programme pour créer l'aide du langage
#our %pri;
$rsortie="$base/chm/html";
$rchm="$base/chm";
mkdir($base);
mkdir($rchm);
mkdir($rsortie);
$rfichier='html';

copy("style.css","$rsortie/style.css");
	
&OuvrirProjet();
&OuvrirSommaire();
&OuvrirIndex();
&EcrireIntro();
&EcrireGrammaire('src/utilisation.txt','Utilisation');
&EcrireGrammaire('src/grammaire.txt','Grammaire');
&EcrireGrammaire('src/technique.txt','Infos techniques');
&EcrireGrammaire('src/motsclefs.txt','Mots-clefs');
&EcrireGrammaire('src/operateurs.txt','Opérateurs');
&OuvrirReferences();
&ChargerMotsClefs('src/motsclefs.txt');
$base="../interpreteur/interne";
$modules="../modules";
&Traiter("$base/chaines.cpp","$base/console.cpp","$base/divers.cpp","$base/hasard.cpp",
	"$base/io.cpp","$base/maths.cpp","$base/systeme.cpp","$base/tableaux.cpp",
	"$modules/chaussettes/chaussettes.cpp","$modules/tortue/tortue.cpp");
&Bas();

sub ChargerMotsClefs
{
	open(F,$_[0]);
	while(<F>)
	{
		if ($_=~/\/\*AIDE/)
		{
			$nom=<F>;
			chomp($nom);
			$hmc{$nom}=1;
		}
	}
	close(F);
}
sub ChargerFonctions
{
	open(F,$_[0]) or die "impossible d'ouvrir '$_[0]'";
	while(<F>)
	{
		if ($_=~/\/\*AIDE/)
		{
			$nom=<F>;
			chomp($nom);
			$hfc{$nom}=1;
		}
	}
	close(F);
}
sub OuvrirSommaire
{
	#On commence le sommaire
	open (G, ">$rchm/Langage.hhc");
	print G <<"ENTETE";
<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">
<HTML>
<HEAD>
<meta name="GENERATOR" content="Cadac par Sidoine">
</HEAD><BODY>
<OBJECT type="text/site properties">
	<param name="ImageType" value="Folder">
</OBJECT>
<UL>
	<LI> <OBJECT type="text/sitemap">
				<param name="Name" value="Introduction">
				<param name="Local" value="$rfichier/intro.html">
				</OBJECT>
ENTETE
	open(GH, ">$rsortie/menu.html");
	print GH <<"ENTETE";
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/DTD/xhtml1-transitional.dtd">
<html>
<head>
	<link rel="stylesheet" type="text/css" href="style.css" />
	<title>Menu</title>
</head>
<body class="menu">
<ul class="Dossier">
	<li class="Fichier"><a href="intro.html" target="txt">Introduction</a></li>
ENTETE
}

sub OuvrirProjet
{
	#On commence le projet
	open (H, ">$rchm/Langage.hhp");
	print H <<"ENTETE";
[OPTIONS]
Compatibility=1.1 or later
Compiled file=Langage.chm
Contents file=Langage.hhc
Default topic=$rfichier/intro.html
Display compile progress=No
Index file=Langage.hhk
Language=0x40c Français (standard)

[FILES]
$rfichier/intro.html
ENTETE
	open(HH,">$rsortie/index.html");
	print HH <<"ENTETE";
<html>
<head><title>Manuel du GOTO++</title></head>
<frameset cols="20%,*">
<frame src="menu.html" />
<frame src="intro.html" id="txt" name="txt" /> 
</frameset>
</html>
ENTETE
	close(HH);
}

sub OuvrirIndex
{
	#On commence l'index
	open (K, ">$rchm/Langage.hhk");
	print K <<"ENTETE";
<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">
<HTML>
<HEAD>
<meta name="GENERATOR" content="Cadac par Sidoine">
</HEAD><BODY>
<UL>
ENTETE
}

sub EcrireIntro
{
	#Ecriture de l'introduction
	open(R,"src/intro.txt");
	@ligne=<R>;
	close(R);
	open(R,">$rsortie/intro.html");
	print R <<"ENTETE";
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<title>Introduction</title>
	<link rel="stylesheet" type="text/css" href="style.css" />
</head>
<body bgcolor="#FFFFFF">
<h1>Introduction</h1>
ENTETE
	print R @ligne;
	print R '</body></html>';
	close(R);
}

sub TraiterListe
{
	my @liste=split(/^-/m,$_[0]);
	shift(@liste);
	return '<ul><li>'.join('</li><li>',@liste).'</li></ul>';
}
sub TraiterTable
{
	my ($ncol,@cell);
	($ncol,@cell)=split(/\n/,$_[0]);
	my $ret='<table><tr>';
	for my $i (0..$ncol-1)
	{
		$ret.="<th>".shift(@cell)."</th>";
	}
	$ret.="</tr>\n";
	my $i=0;
	while (scalar @cell)
	{
		$ret.='<tr>' if ($i==0);
		$ret.='<td>'.shift(@cell).'</td>';
		$i++;
		if ($i==$ncol)
		{
			$ret.="</tr>\n";
			$i=0;
		}
	}
	return $ret.'</table>';
}

sub EcrireGrammaire
{
	print G <<"ENTETE";
	<LI> <OBJECT type="text/sitemap">
		<param name="Name" value="$_[1]">
		</OBJECT>
	<UL>
ENTETE

	print GH <<"ENTETE";
	<li class="Dossier">$_[1]
		<ul>
ENTETE
	my @ligne;
	#Ecriture de la grammaire
	open(R,"$_[0]");
	@ligne=<R>;
	close(R);
	my $ou=0;
	my $texte;
	for my $ligne (@ligne)
	{
		if ($ligne=~/^\$/)
		{
			chomp($ligne);
			$ligne=~s/\$//;
			if ($ou)
			{
				$texte=~s/&/&amp;/gs;
				$texte=~s/</&lt;/gs;
				$texte=~s/>/&gt;/gs;
				$texte=~s/\n\n/<\/p><p>/gs;
				$texte=~s/#exemple(.*?)exemple#/'<\/p><pre class=\"exemple\">'.&Illuminer($1,'anticonstitu').'<\/pre><p>'/gse;
				$texte=~s/#titre(.*?)titre#/<h2>$1<\/h2>/gs;
				$texte=~s/#titre2(.*?)titre2#/<h3>$1<\/h3>/gs;
				$texte=~s/#liste(.*?)liste#/&TraiterListe($1)/gse;
				$texte=~s/#table\s*(.*?)\s*table#/&TraiterTable($1)/gse;
				$texte=~s/£(.*?)£/<b>$1<\/b>/gs;
				$texte=~s/\$(.*?)\$/'<a href="'.&url_code($1).".html\">$1<\/a>"/gse;
				print S $texte;
				$texte='';
				print S '</p></body></html>';
				close(S);
			}
			$ou=$ligne;
			if ($ou)
			{
				$ouc=&url_code($ou);
				open(S,">$rsortie/$ouc.html");
				print S <<"ENTETE";
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<title>$ou</title>
	<link rel="stylesheet" type="text/css" href="style.css" />
</head>
<body bgcolor="#FFFFFF">
<h1>$ou</h1>
<p>
ENTETE
				print H "$rfichier/$ouc.html\n";
				print G <<"PAGE";
				<LI> <OBJECT type="text/sitemap">
				<param name="Name" value="$ou">
				<param name="Local" value="$rfichier/$ouc.html">
				</OBJECT>
PAGE
				print GH <<"PAGE";
				<li><a href="$ouc.html" target="txt">$ou</a></li>
PAGE
			}
		}
		else
		{$texte.=$ligne;}
	}
	print G "</UL></LI>\n";
	print GH "</ul></li>\n";
}

sub OuvrirReferences
{
	#Ecriture du livre "Références"
	print G <<"REF";
		<LI> <OBJECT type="text/sitemap">
		<param name="Name" value="Références">
		</OBJECT>
		<UL>
REF
	print GH <<"REF";
		<li>Références
			<ul>
REF
}

sub TablePriorites
{
	#Table des priorités
	print G <<"ORDRE";
<LI> <OBJECT type="text/sitemap">
		<param name="Name" value="Table des priorités">
		<param name="Local" value="$rfichier/priorite.html">
		</OBJECT></LI>
ORDRE

	print GH <<"ORDRE";
<li><a href="priorite.html" target="txt">Table des priorités</a></li>
ORDRE
	
	print H "$rfichier/priorite.html\n";

	open (F,">$rsortie/priorite.html");
	print F <<"ENTETE";
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<title>Table des priorités</title>
	<link rel="stylesheet" type="text/css" href="style.css" />
</head>
<body bgcolor="#FFFFFF">
<h1>Table des priorités</h1>
<p>Seules les fonctions ayant une priorité différente de 10 et les opérateurs et mots-clefs
ayant une priorité différente de 1 sont mentionnées dans cette table.
Toutes les autres fonctions décrites dans ce manuel ont une priorité de 10. Tous les autres mots-clefs
et les autres opérateurs ont une priorité de 1.
Toutes les constantes, les variables ou fonctions que vous définissez ont une
priorité de 1.</p>
<table>
<tr><th>Fonction</th><th>Priorité</th></tr>
ENTETE

	foreach $fonc (sort {$pri{$b} <=> $pri{$a}} keys %pri)
	{
		$cfonc=&url_code($fonc);
		print F "<tr><td><a href=\"$cfonc.html\">$fonc</a></td><td>$pri{$fonc}</td></tr>\n";
	}
	print F '</table></body></html>';
	close(F);
}

sub Bas
{
	#Bas de pages
	print G "	</UL></LI>\n";
	print G "</UL>\n</BODY></HTML>";
	close(G);

	print GH "</ul></li>\n";
	print GH "</ul>\n</body></html>";
	
	print H "\n[INFOTYPES]\n";
	close(H);

	print K "</UL>\n</BODY></HTML>";
	close(K);
}

sub url_decode
{
	local ($s) = @_;

	$s =~ tr/+/ /;
	$s =~ s/%([0-9A-F][0-9A-F])/pack("C",oct("0x$1"))/ge;
	$s;
}

sub url_code 
{
	local ($s) = @_;

	$s =~ s/([^\w\s])/'~'.sprintf('%X',ord($1))/ge;
	$s =~ tr/ /+/;
	$s;
}

sub ClasseFromNom
{
	my $classe=$_[0];
	my $no;
	if ($classe=~/%/)
	{
		$classe=~s/%(\w+)$//;
		$no=$1;
	}
	else
	{
		$classe='global';
		$no=$_[0];
	}
	return ($classe,$no);
}

sub TraiterFichier
{
	open(F,$_[0]) || die "impossible de trouver $_[0]";
	undef $/;
	my $fichier=<F>;
	$/="\n";
	close(F);
	chomp(@ligne);
	
	while ($fichier=~m#/\*CLASSE(.*?)\*/#gs)
	{
		my $entree=$1;
		$entree=~s/&/&amp;/gs;
		$entree=~s/</&lt;/gs;
		my @morceaux=split(/\n\\/s,$entree);
		my $desc=shift(@morceaux);
		$desc=~s/\s*([\w%]+)\s*//s;
		my $nom=$1;
		my ($classe,$no)=&ClasseFromNom($nom);
		push(@{$cat{$classe}},$no);
		for my $morceau (@morceaux)
		{
			$morceau=~s/(\w+)\s*//s;
			my $tag=$1;
			$morceau=~s/\s*$//s;
			my $module;
			if ($tag eq 'module')
			{
				$module=$tag;
			}
			else
			{
				die "tag $tag inconnu";
			}
		}
		$classe{$nom}{desc}=$desc;
		$classe{$nom}{module}=$module;
	}
	
	while ($fichier=~m#/\*AIDE(.*?)\*/#gs)
	{
		my @param;
		my $entree=$1;
		$entree=~s/&/&amp;/gs;
		$entree=~s/</&lt;/gs;
		my @morceaux=split(/\n\\/s,$entree);
		my $voir;
		my $desc=shift(@morceaux);
		$desc=~s/\s*([\w%]+)\s*//s;
		my $nom=$1;
		my ($classe,$no)=&ClasseFromNom($nom);
		my @return;
		my $etc=0;
		my $static=0;
		my $longdesc='';
		my $exemple='';
		my $module;
		for my $morceau (@morceaux)
		{
			$morceau=~s/(\w+)\s*//s;
			my $tag=$1;
			$morceau=~s/\s*$//s;
			if ($tag eq 'param')
			{
				$morceau=~s/(\w+)\s*(\w+)\s*//s;
				push(@param,[$1,$2,$morceau]);
			}
			elsif ($tag eq 'retour')
			{
				$morceau=~s/(\w+)\s*//s;
				@return=($1,$morceau);
			}
			elsif ($tag eq 'voir')
			{
				$voir=$morceau;
			}
			elsif ($tag eq 'exemple')
			{
				$exemple=$morceau;
			}
			elsif ($tag eq 'desc')
			{
				$longdesc=$morceau;
			}
			elsif ($tag eq 'module')
			{
				$module=$morceau;
			}
			elsif ($tag eq 'etc')
			{
				$etc=1;
			}
			elsif ($tag eq 'static')
			{
				$static=1;
			}
			else
				{die "tag $tag inconnu";}
		}
		$static=1 if ($classe eq 'global');
	
		#print E $nom,"\n";
		$desc=~s/\$(.*?)\$/'<a href="'.&url_code($1).".html\">$1<\/a>"/gse;
		$desc=~s/\b(\Q$nom\E)\b/<b>$1<\/b>/gs;
				
		open(F,">$rsortie/".url_code($nom).'.html');
		print F <<"ENTETE";
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<title>$nom</title>
	<link rel="stylesheet" type="text/css" href="style.css" />
</head>
<body bgcolor="#FFFFFF">
<h1>$nom</h1>
<p>$desc</p>
<pre class="syntaxe">
ENTETE
		
		print F $no,' ';
		print F "(";
			
		if (scalar @param)
		{
			print F "\n";
			for my $param (@param)
			{
				my @p=@{$param};
				print F "\t $p[1] <a href=\"#e$p[0]\">$p[0]</a>\n";
			}
			
		}
		print F "..." if ($etc);
		print F ') ';
		print F ':<a href="'.&url_code($classe).".html\">$classe</a>" unless ($static);
		if (@return)
		{
			print F "\n&rarr;";
			print F "\t <a href=\"#sreturn\">$return[0]</a>\n";
		}
		print F "\n</pre>";

		print F "<h4>Membre de</h4>\n<p>";
		print F '<p><b><a href="'.&url_code($classe).".html\">$classe</a></b>";
		
		if (scalar @param)
		{
			print F "<h4>Paramètres d'entrée</h4>\n<dl>\n";
			for my $param (@param)
			{
				my @p=@{$param};
				print F "\t<dt><a name=\"#e$p[0]\" />$p[0]</a></dt>\n<dd>$p[2]</dd>";
			}
			print F "</dl>\n";
		}
		if (scalar @return)
		{
			print F "<h4>Retour</h4>\n<dl>\n";
			
			$return[1]=~s/£(.*?)£/<a href="#e$1">$1<\/a>/g;
			print F "\t<dt><a name=\"#sreturn\" />$return[0]</a></dt>\n<dd>$return[1]</dd>";
			print F "</dl>\n";
		}
		if ($longdesc)
		{
			$longdesc=~s/\n\n/<\/p><p>/gs;
			$longdesc=~s/¤(.*?)¤/<a href="#s$1">$1<\/a>/gs;
			$longdesc=~s/£(.*?)£/<a href="#e$1">$1<\/a>/gs;
			$longdesc=~s/\b(\Q$nom\E)\b/<b>$1<\/b>/gs;
			$longdesc=~s/\$(.*?)\$/'<a href="'.&url_code($1).".html\">$1<\/a>"/gse;
			print F "<h4>Remarques</h4>\n<p>$longdesc</p>\n";
		}
		if ($exemple)
		{
			$exemple=&Illuminer($exemple,$nom);
			print F "<h4>Exemple</h4>\n<pre class=\"exemple\">$exemple</pre>\n";
		}
		
		print F "<h4>Voir aussi</h4>\n<p>";
		print F '<p><b><a href="'.&url_code($classe).".html\">$classe</a></b>";
		if ($voir)
		{
			@voir=split(',',$voir);
			@voir=sort @voir;
			for $v (@voir)
			{
				print F ', <a href="'.&url_code($v).".html\">$v</a>";
			}
		}
		print F "</p>\n";
		
		if ($module)
		{
			print F "<h4>Module</h4>\n<p>";
			print F "<p>GOTOMODULE $module</b>";
		}
				
		push(@{$cat{$classe}},$no);
		
		print F "</body>\n</html>";
		close(F);
	}
}

sub Traiter
{
	for my $fichier (@_)
	{
		&TraiterFichier($fichier);
	}
	
	#On ouvre le livre
	print G <<"TYPE";
	<LI> <OBJECT type="text/sitemap">
			<param name="Name" value="doc">
			</OBJECT>
		<UL>
TYPE
	print GH <<"TYPE";
	<li>Fonctions
		<ul>
TYPE
	
	#Ecriture des catégories de référence
	foreach my $c (sort {$a cmp $b} keys %cat)
	{
		my $cc=&url_code($c);
		print H "$rfichier/$cc.html\n";
		print G <<"CATEG";
		<LI> <OBJECT type="text/sitemap">
			<param name="Name" value="$c">
			<param name="Local" value="$rfichier/$cc.html">
			</OBJECT>
		<UL>
CATEG
		print GH <<"CATEG";
		<li><a href="$cc.html" target="txt">$c</a>
			<ul>
CATEG
		open(F,">$rsortie/".&url_code($c).'.html');
		print F <<"ENTETE";
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<title>$c</title>
	<link rel="stylesheet" type="text/css" href="style.css" />
</head>
<body bgcolor="#FFFFFF">
<h1>$c</h1>
<p>$classe{$c}{desc}</p>
<ul>
ENTETE

		@machin=@{$cat{$c}};
		foreach $no (sort {$a cmp $b} @machin)
		{
			my $nom;
			if ($c eq 'global')
				{$nom=$no;}
			else
				{$nom="$c%$no";}
			my $code=&url_code($nom);
			print H "$rfichier/$code.html\n";
			print F "<li><a href=\"$code.html\">$nom</a></li>\n";
			print G <<"PAGE";
			<LI> <OBJECT type="text/sitemap">
				<param name="Name" value="$no">
				<param name="Local" value="$rfichier/$code.html">
				</OBJECT>
PAGE
		print GH <<"PAGE";
			<li><a href="$code.html" target="txt">$no</a></li>
PAGE
		print K <<"MOTCLEF";
	<LI> <OBJECT type="text/sitemap">
		<param name="Name" value="$no">
		<param name="Local" value="$richier/$code.html">
		</OBJECT>
MOTCLEF
		}
		print F "</ul>\n";
		if ($classe{$c}{module})
		{
			print F "<h4>Module</h4><p>$classe{$c}{module}</p>\n";
		}
		print F "</body>\n</html>";
		print G "		</UL></LI>\n";
		print GH "		</ul></li>\n";
		close(F);
	}
	#On ferme le livre
	print G "</UL>\n";
	print GH "</ul>\n";
}
				
sub Illuminer
{
	my $exemple=$_[0];
	$exemple=~s/<\/p><p>/\n\n/gs;
	$exemple=~s/(\Q$_[1]\E)/<span class="fonction">$1<\/span>/gs;
	$exemple=~s/(§[\d\w]+)/<span class="etiquette">$1<\/span>/gs;
	$exemple=~s/(%[\d\w]+)/<span class="nombre">$1<\/span>/gs;
	$exemple=~s/(\?[\d,]+)/<span class="nombre">$1<\/span>/gs;
	$exemple=~s/(\*`.*')/<span class="carac">$1<\/span>/gs;	
	$exemple=~s/(\*\(.*?\))/<span class="nombre">$1<\/span>/gs;
	$exemple=~s/«(.*?)»/<span class="chaine">&laquo;$1&raquo;<\/span>/gs;
	$exemple=~s/(\#!.*?!\#)/<span class="commentaire">$1<\/span>/gs;
	$exemple=~s/(\bGOTO\s.*)/<span class="commentaire">$1<\/span>/g;
	#$exemple=~s/(\bpingouin\b)/<span class="syntaxe">$1<\/span>/gs;
	$exemple=~s/\b(pingouin|troupeau|fintroupeau|ASV|APLUS|machineapingouins|GIVRE)\b/<span class="syntaxe">$1<\/span>/gs;
	return $exemple;
}
