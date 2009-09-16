close all;
clear all;
more off;



PB = 0.0:0.1:1.0;

function C = CONF(c)
  switch (c)
    case 1
      C = ' 1:1';
    case 2
      C = ' 2:1';
    case 3
      C = '10:1';
  end % switch
endfunction

%------------------------------------------------------------------------------
% configuration 1:1, 2:1, 10:1
%------------------------------------------------------------------------------
if (1)
replan = load('numrobots_replan.dat');
wait = load('numrobots_wait.dat');

for c = 1:3
  for pb = 1:11
    fprintf('%s %0.1f: replan %.1f %.1f  wait %.1f %.1f\n', CONF(c), PB(pb),
            mean( replan.meanTaskA(c,:)), 
            mean( replan.meanTaskB(c,:)),
            mean( wait.meanTaskA(c,pb,:)),
            mean( wait.meanTaskB(c,pb,:)) );
  end % for pb
end % for c
end
%------------------------------------------------------------------------------
% configuration 3:1 -> 1:3
%------------------------------------------------------------------------------
clear replan;
clear wait;
replan = load('numrobots_replan_3:1-1:3.dat');
wait = load('numrobots_wait_3:1-1:3.dat');

for pb = 1:11
  fprintf('3:1->1:3 %0.1f: replan %.1f %.1f %.1f %.1f  wait %.1f %.1f %.1f %.1f\n', PB(pb),
          mean( replan.meanTaskA_1(:)), 
          mean( replan.meanTaskB_1(:)),
          mean( replan.meanTaskA_2(:)), 
          mean( replan.meanTaskB_2(:)),

          mean( wait.meanTaskA_1(pb,:)),
          mean( wait.meanTaskB_1(pb,:)),
          mean( wait.meanTaskA_2(pb,:)),
          mean( wait.meanTaskB_2(pb,:)) );

end % for pb