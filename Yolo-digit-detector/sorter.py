from shutil import copyfile
import os
true_pics = os.listdir('imgs_validation')


#for filename in os.listdir('imgs'):
#    if filename not in true_pics:
#        os.remove('imgs/'+filename)

for filename in os.listdir('ann'):
    if filename.split('.')[0]+'.jpg' in true_pics:
        copyfile(os.path.join('ann',filename), 'ann_validation/'+filename)
        os.remove('ann/'+filename.split('.')[0]+'.xml')

