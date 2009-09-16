close all;
clear all;
more off;

PB=0.0:0.1:1.0;

function C = CONF(c)
  switch (c)
    case 1
      C = '1_1';
    case 2
      C = '2_1';
    case 3
      C = '10_1';
  end % switch
endfunction

FONTSIZE = 24;
MS = 11;
%------------------------------------------------------------------------------
% Plot energy work graphs
%------------------------------------------------------------------------------
if (0)

  fixed = load('energy_flags_fixed.dat');
  replan = load('energy_flags_replan.dat');
  wait = load('energy_flags_wait.dat');

  for c = 1:3
    for pb = 1:11
      figure;
      plot(fixed.fixed_energy(c,:), fixed.fixed_flags(c,:), 'go;fixed;',
           "markersize", 9);
      hold on;
      plot(wait.wait_energy(c,pb,:), wait.wait_flags(c,pb,:),'kx',
           "markersize", MS);
      plot(wait.wait_energy(c,pb,1), wait.wait_flags(c,pb,1),'kx;heuristic;',
           "markersize", MS);
      plot(replan.replan_energy(c,:), replan.replan_flags(c,:), 'r*;replan;',
           "markersize", MS);
      hold off;
      legend("location", "southeast");
      axis([0 3500 0 399]);
      %grid on;
      %ylabel('pucks', 'fontsize', FONTSIZE);
      %xlabel('energy', 'fontsize', FONTSIZE);
      %title(sprintf('conf %s pb %f', CONF(c), PB(pb) ));
      name = sprintf('energy_flags_%s_%02d.eps', CONF(c), pb-1 );
      print(name, '-depsc2', '-F:24' );
    end % for pb
  end % for c
end % if

%------------------------------------------------------------------------------
% Plot energy work graphs for 3:1 -> 1:3
%------------------------------------------------------------------------------
if (1)
  conf = '3_1-1_3';
  replan = load('energy_flags_replan_3:1-1:3.dat');
  wait = load('energy_flags_wait_3:1-1:3.dat');

  for pb = 1:11
    figure;

    plot(replan.replan_energy(:), replan.replan_flags(:), 'r*;replan;',
         "markersize", MS);
    hold on;
    plot(wait.wait_energy(pb,1), wait.wait_flags(pb,1),'kx;heuristic;',
         "markersize", MS);
    plot(wait.wait_energy(pb,:), wait.wait_flags(pb,:),'kx', 
         "markersize", MS);
    hold off;
    legend("location", "southeast");
    axis([0 3500 0 399]);
    %grid on;
    ylabel('pucks', 'fontsize', FONTSIZE);
    xlabel('energy', 'fontsize', FONTSIZE);
    %title(sprintf('pb %f', PB(pb) ));
    name = sprintf('energy_flags_%s_%02d.eps', conf, pb-1 );
    print(name, '-depsc2', '-F:24' );
  end % for pb
end % if