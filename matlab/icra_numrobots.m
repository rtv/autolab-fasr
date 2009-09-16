close all;
clear all;
more off;

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
% load replan data
if (1)
  for c = 1:3
    fprintf('replan %s \n', CONF(c) );
    for i = 1:20
      replan = load(sprintf('../data/replan_%s_%d.dat',CONF(c), i) );

      meanTaskA(c,i) = mean( replan(:,T1_ROBOTS) );
      stdTaskA(c,i) = mean( replan(:,T1_ROBOTS) );

      meanTaskB(c,i) = mean( replan(:,T2_ROBOTS) );
      stdTaskB(c,i) = mean( replan(:,T2_ROBOTS) );
    end % for i
  end % for c
  save('numrobots_replan.dat',"meanTaskA", "stdTaskA", "meanTaskB", "stdTaskB");
end % if

%------------------------------------------------------------------------------
% load wait data
if (1)
  for c = 1:3
    for pb = 1:11
      fprintf('wait %s prob %f \n', CONF(c), PB(pb) );
      for i = 1:20
        wait = load(sprintf('../data/wait_%s_%d_%0.1f.dat',CONF(c),i, PB(pb)));

        meanTaskA(c,pb,i) = mean( wait(:,T1_ROBOTS) );
        stdTaskA(c,pb,i) = mean( wait(:,T1_ROBOTS) );

        meanTaskB(c,pb,i) = mean( wait(:,T2_ROBOTS) );
        stdTaskB(c,pb,i) = mean( wait(:,T2_ROBOTS) );
      end % for i
    end % for pb
  end % for c

  save('numrobots_wait.dat', "meanTaskA", "stdTaskA", "meanTaskB", "stdTaskB");
end % if


%------------------------------------------------------------------------------
% load 3:1 -> 1:3 step replan data
T_SWITCH = 36000;
FINI = 72000;

if (1)
  fprintf('3:1 -> 1:3 replan\n');
  for i = 1:20
    replan = load(sprintf('../data/replan_3:1-1:3_%d.dat', i) );

    meanTaskA_1(i) = mean( replan(1:T_SWITCH, T1_ROBOTS) );
    stdTaskA_1(i) = mean( replan(1:T_SWITCH, T1_ROBOTS) );

    meanTaskA_2(i) = mean( replan(T_SWITCH:FINI, T1_ROBOTS) );
    stdTaskA_2(i) = mean( replan(T_SWITCH:FINI, T1_ROBOTS) );

    meanTaskB_1(i) = mean( replan(1:T_SWITCH, T2_ROBOTS) );
    stdTaskB_1(i) = mean( replan(1:T_SWITCH, T2_ROBOTS) );

    meanTaskB_2(i) = mean( replan(T_SWITCH:FINI, T2_ROBOTS) );
    stdTaskB_2(i) = mean( replan(T_SWITCH:FINI, T2_ROBOTS) );
  end % for i

  save('numrobots_replan_3:1-1:3.dat',"meanTaskA_1", "stdTaskA_1", "meanTaskA_2", "stdTaskA_2", "meanTaskB_1", "stdTaskB_1", "meanTaskB_2", "stdTaskB_2");
end % if

%------------------------------------------------------------------------------
%  load 3:1 -> 1:3 step wait data
if (1)
  for pb = 1:11
    fprintf('3:1 -> 1:3 wait %0.1f\n', PB(pb) );
    for i = 1:20
      wait = load(sprintf('../data/wait_3:1-1:3_%d_%0.1f.dat', i, PB(pb)));

      % task A before the switch
      meanTaskA_1(pb,i) = mean( wait(1:T_SWITCH, T1_ROBOTS) );
      stdTaskA_1(pb,i) = mean( wait(1:T_SWITCH, T1_ROBOTS) );

      % task A after the switch
      meanTaskA_2(pb,i) = mean( wait(T_SWITCH:FINI, T1_ROBOTS) );
      stdTaskA_2(pb,i) = mean( wait(T_SWITCH:FINI, T1_ROBOTS) );

      % task B before the switch
      meanTaskB_1(pb,i) = mean( wait(1:T_SWITCH, T2_ROBOTS) );
      stdTaskB_1(pb,i) = mean( wait(1:T_SWITCH, T2_ROBOTS) );

      % task B after the switch
      meanTaskB_2(pb,i) = mean( wait(T_SWITCH:FINI, T2_ROBOTS) );
      stdTaskB_2(pb,i) = mean( wait(T_SWITCH:FINI, T2_ROBOTS) );

    end % for i
  end % for pb
 
  save('numrobots_wait_3:1-1:3.dat', "meanTaskA_1", "stdTaskA_1", "meanTaskA_2", "stdTaskA_2", "meanTaskB_1", "stdTaskB_1", "meanTaskB_2", "stdTaskB_2");
end % if