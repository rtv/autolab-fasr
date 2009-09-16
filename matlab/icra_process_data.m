close all;
clear all;
more off;
fprintf('\n');


function C = CONF(c)
  switch (c)
    case 1
      C = '1:1';
    case 2
      C = '2:1';
    case 3
      C = '10:1';
  end % switch
endfunction

DEPOT         =  1;
CHARING       =  2;
TASK_SWITCH   =  3;
REWARD        =  4;
FLAGS         =  5;
ENERGY        =  6;
T1_ROBOTS     =  7;
T1_OPT_ROBOTS =  8;
T1_RTT        =  9;
T1_RATE       = 10;
T1_REWARD     = 11;
T1_TASKS      = 12;
T2_ROBOTS     = 13;
T2_OPT_ROBOTS = 14;
T2_RTT        = 15;
T2_RATE       = 16;
T2_REWARD     = 17;
T2_TASKS      = 18;

PB = 0.0:0.1:1.0;

%------------------------------------------------------------------------------
% load bruteforce data 1:1, 2:1, 10:1
if (0)
  for c = 1:3
    CONF(c)
    for g = 0:18
      for b = 0:18
        fixed = load(sprintf('../data/fixed_%s_%d_%d.dat', CONF(c), g, b) ); 
        [N M] = size(fixed);

        fixed_energy(c,g+1,b+1) = fixed(N, ENERGY);
        fixed_flags(c,g+1,b+1) = fixed(N, FLAGS);

      end % for b
    end % for g

  end % for c
  save energe_flags_fixed.dat fixed_energy fixed_flags
end % if

%------------------------------------------------------------------------------
% load replan data 1:1, 2:1, 10:1
if (0)
  for c = 1:3
    for i = 1:20
      replan = load(sprintf('../data/replan_%s_%d.dat',CONF(c), i) );
      [N M] = size(replan);
      replan_energy(c,i) = replan(N, ENERGY); 
      replan_flags(c,i) = replan(N, FLAGS);
    end % for i
  end % for c
  save energy_flags_replan.dat replan_energy replan_flags
end % if

%------------------------------------------------------------------------------
% load wait data 1:1, 2:1, 10:1
if (0)
  for c = 1:3
    for pb = 1:11
      fprintf('wait %s prob %f \n', CONF(c), PB(pb) );
      for i = 1:20
        wait = load(sprintf('../data/wait_%s_%d_%0.1f.dat',CONF(c),i, PB(pb)));
        [N M] = size(wait);
        wait_energy(c,pb,i) = wait(N, ENERGY); 
        wait_flags(c,pb,i) = wait(N, FLAGS);
      end % for i
    end % for pb
  end % for c
  save energy_flags_wait.dat wait_energy wait_flags
end % if

%------------------------------------------------------------------------------
% load replan data 3:1 -> 1:3
if (0)
  conf = '3:1-1:3';
  for i = 1:20
    replan = load(sprintf('../data/replan_%s_%d.dat',conf, i) );
    [N M] = size(replan);
    replan_energy(i) = replan(N, ENERGY); 
    replan_flags(i) = replan(N, FLAGS);
  end % for i

  filename = sprintf('energy_flags_replan_%s.dat', conf);
  save(filename, "replan_energy", "replan_flags");
end % if

%------------------------------------------------------------------------------
% load wait data 3:1 -> 1:3
if (1)
  conf = '3:1-1:3';
  for pb = 1:11
    fprintf('wait %s prob %f \n', conf, PB(pb) );
    for i = 1:20
      wait = load(sprintf('../data/wait_%s_%d_%0.1f.dat', conf, i, PB(pb)));
      [N M] = size(wait);
      wait_energy(pb,i) = wait(N, ENERGY); 
      wait_flags(pb,i) = wait(N, FLAGS);
    end % for i
  end % for pb

  filename = sprintf('energy_flags_wait_%s.dat', conf);
  save(filename, "wait_energy", "wait_flags");
end % if