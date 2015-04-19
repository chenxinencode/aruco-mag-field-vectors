function quiver3AROneVector(x,y,z,u,v,w)
    fid = fopen('demo.yml','w');
    tot =       '%%YAML:1.0\n'; % this will output as a single percent sign.
    tot = [tot, '    oneVec: !!opencv-matrix\n'];
    tot = [tot, '       rows: 1\n'];
    tot = [tot, '       cols: 6\n'];
    tot = [tot, '       dt: f\n'];
    
    
    fprintf(fid,tot);
    
    
    
    % and finish with...
    fprintf(fid,'       data: [%f, %f, %f, %f, %f, %f]\n', ...
        x,y,z,u,v,w);



    fclose(fid);
end