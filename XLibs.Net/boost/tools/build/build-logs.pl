#!/usr/bin/perl

#~ Copyright 2003, Rene Rivera.
#~ Use, modification and distribution are subject to the Boost Software
#~ License Version 1.0. (See accompanying file LICENSE_1_0.txt or
#~ http://www.boost.org/LICENSE_1_0.txt)

use FileHandle;
use Time::Local;

my $logdir = "$ENV{PWD}";
#~ my $logdir = "C:\\CVSROOTs\\Boost\\boost";
#~ open STDOUT,">C:\\CVSROOTs\\Boost\\boost\\build-logs.html";
opendir LOGS, "$logdir";
my @logs = grep /build-.*log$/, readdir LOGS;
closedir LOGS;
my @bgcolor = ( "bgcolor=\"#EEEEFF\"", "" );
print "<table>\n";
print "<tr>\n",
    "<th align=\"left\" bgcolor=\"#DDDDDD\">Build</th>\n",
    "<th align=\"left\" bgcolor=\"#DDDDDD\">Library</th>\n",
    "<th align=\"left\" bgcolor=\"#DDDDDD\">Warnings</th>\n",
    "<th align=\"left\" bgcolor=\"#DDDDDD\">Failures</th>\n",
    "</tr>\n";
foreach $l (sort { lc($a) cmp lc($b) } @logs)
{
    my $log = $l;
    my $fh = new FileHandle;
    if ($fh->open("<$logdir/$log"))
    {
        my $content = join('',$fh->getlines());
        $fh->close;
        print
            "<tr>\n",
            "  <td valign=\"top\"><a href=\"",$log,"\">",$log,"</a></td>\n";
        my %failures;
        my %warnings;
        my $action_regex = "\\n[\\w\\+\\-]+\\s(?:bin|stage)";
        my @actions = ($content =~ /($action_regex(?:.(?!$action_regex))*.)/gis);
        my $action_target;
        foreach my $a (@actions)
        {
            my ($at) = ($a =~ /$action_regex.boost.libs.([^\\\/]*.[^\\\/]*.[^\\\/]*)/gi);
            my ($action) = ($a =~/\n([\w\+\-]+)/);
            if ($at) { $action_target = $at; }
            my ($lib) = ($action_target =~ /([^\\\/]*)/i);
            $failures{$lib} += 0;
            $warnings{$lib} += 0;
            my ($f) = ($a =~ /\n\.\.\.failed\s([^\s]+)/is);
            if ($f && $f eq "$action") { $failures{$lib} += 1; }
            $warnings{$lib} += scalar ($a =~ /(warning(?:\s#\d+)?\:)/gis);
        }
        my @libs = sort(keys %failures);
        foreach my $l (0..scalar(@libs)-1)
        {
            print
                "  <td ",$bgcolor[$l %2],">",$libs[$l],"</td>\n",
                "  <td ",$bgcolor[$l %2],">";
            if ($warnings{$libs[$l]} > 0) { print $warnings{$libs[$l]}; }
            print "</td>\n",
                "  <td ",$bgcolor[$l %2],"><strong><font color=\"#FF0000\">";
            if ($failures{$libs[$l]} > 0) { print $failures{$libs[$l]}; }
            else { print "&nbsp;"; }
            print "</font></strong></td>\n",
                "</tr>\n";
            if ($l < scalar(@libs))
            {
                print
                    "<tr>\n",
                    "  <td></td>\n";
            }
        }
        print "<tr><td colspan=\"4\"><hr></td></tr>\n";
    }
}
print "</table>\n";
