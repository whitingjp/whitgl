#!/usr/bin/python

import struct
import argparse
import sys
import os.path

def process_mtl(filename):
	materials = []
	file = open(filename)
	for line in file:
		tokens = line.split()
		if(len(tokens) == 0):
			continue
		ident = tokens.pop(0)
		if(ident == 'newmtl'):
			m = {}
			m['name'] = tokens[0]
			m['color'] = (1,1,1)
			materials.append(m)
		if(ident == 'Kd'):
			materials[-1]['color'] = (float(tokens[0]),float(tokens[1]),float(tokens[2]))
	return materials

def process_obj(filename):
	file = open(filename)
	vertices = []
	normals = []
	faces = []
	materials = []
	default_material = {}
	default_material['name'] = 'default'
	default_material['color'] = (1,0.1,1)
	materials.append(default_material)
	current_material = 0
	for line in file:
		tokens = line.split()
		if(len(tokens) == 0):
			continue
		ident = tokens.pop(0)
		if(ident == 'v'):
			vertex = (float(tokens[0]),float(tokens[1]),float(tokens[2]));
			vertices.append(vertex)
		if(ident == 'vn'):
			normal = (float(tokens[0]),float(tokens[1]),float(tokens[2]));
			normals.append(normal)
		if(ident == 'f'):
			face = (int(tokens[0].split('/')[0]),int(tokens[1].split('/')[0]),int(tokens[2].split('/')[0]),current_material)
			faces.append(face)
			if len(tokens) == 4:
				face = (int(tokens[2].split('/')[0]),int(tokens[3].split('/')[0]),int(tokens[0].split('/')[0]),current_material)
				faces.append(face)
		if(ident == 'mtllib'):
			path = os.path.join(os.path.dirname(filename), tokens[0])
			materials += process_mtl(path)
		if(ident == 'usemtl'):
			current_material = 0
			for i in range(len(materials)):
				if materials[i]['name'] == tokens[0]:
					current_material = i
	return vertices, normals, faces, materials


def main():
	parser = argparse.ArgumentParser(description='Convert a wavefront obj file to use in slicer.')
	parser.add_argument('src', help='obj file name')
	parser.add_argument('dst', help='wmd file name')

	args = parser.parse_args()
	print("Converting %s to %s" % (args.src, args.dst))

	vertices, normals, faces, materials = process_obj(args.src)

	vertices_size = len(faces)*3*3*4;
	colours_size = vertices_size * 2;
	size = vertices_size + colours_size
	print ("Vertices %d size %d" % (len(faces)*3, size))

	out = open(args.dst, 'wb')
	out.write(struct.pack('i', size))
	for face in faces:
		m = materials[face[3]]
		for index in face[:3]:
			vertex = vertices[index-1]
			normal = normals[index-1]
			for f in vertex:
				out.write(struct.pack('f', f))
			for c in m['color']:
				out.write(struct.pack('f', c))
			for n in normal:
				out.write(struct.pack('f', n))

if __name__ == "__main__":
    main()
