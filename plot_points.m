# Prevent Octave from thinking that this
# is a function file:
1;
# Define function triangulate:
function triangulate (filepath)
    clf;
    points = csvread(filepath);
    x = points(1:size(points));
    y = points(size(points)+1:size(points)*2);
    tri = delaunay (x, y);
    triplot (tri, x, y);
    hold on;
    plot (x, y, "r.");
    outname = strcat(filepath, ".jpg");
    print(outname,"-djpg");
endfunction

# Get arguments
args = argv();
for i = 1:rows(args)
  for j = 1:columns(args)
    # For each argument execute triangulate
    triangulate(args{i,j});
  endfor
endfor
